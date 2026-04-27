#include <memory>

#include "Player.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/trigonometric.hpp>
#include <imgui.h>

#include "Bullet.h"
#include "GameConstants.h"
#include "Monad.h"
#include "animation/IdleClip.h"
#include "animation/SkeletalAnimationClip.h"
#include "animation/Skeleton.h"
#include "editor/Editor.h"
#include "physics/Collider.h"
#include "physics/RigidBody.h"
#include "render/Material.h"
#include "render/Mesh.h"
#include "scene/GameObject.h"
#include "scene/components/AnimationComponent.h"
#include "scene/components/AudioComponent.h"
#include "scene/components/MeshComponent.h"
#include "scene/components/PhysicsComponent.h"
#include "scene/components/PlayerControllerComponent.h"

using namespace mnd;

namespace
{
f32 EaseOutCubic(f32 x)
{
    f32 inv = 1.0f - x;
    return 1.0f - inv * inv * inv;
}

f32 EaseInQuad(f32 x)
{
    return x * x;
}
}  // namespace

void Player::Init()
{
    if (auto bullet = FindChildByName(kChildBulletName))
    {
        bullet->SetActive(false);
    }

    if (auto fire = FindChildByName(kChildBoomName))
    {
        fire->SetActive(false);
    }

    m_gunObject = FindChildByName(kChildGunName);
    if (m_gunObject)
    {
        m_animationComponent = m_gunObject->GetComponent<mnd::AnimationComponent>();
    }

    m_handsObject = FindChildByName(kChildHandsName);
    if (m_handsObject)
    {
        m_handsAnimComponent = m_handsObject->GetComponent<mnd::AnimationComponent>();

        // Sword.glb is a static mesh — apply rest pose from tweaker defaults so
        // we have a known orientation to blend attack/block animations against.
        m_handsObject->SetPosition(m_handsPosition);
        m_handsObject->SetRotation(glm::quat(glm::radians(m_handsRotEuler)));
        m_handsObject->SetScale(mnd::vec3(m_handsScale));

        m_swordRestPos = m_handsObject->GetPosition();
        m_swordRestRot = m_handsObject->GetRotation();
    }

    SetViewmodel(/*useHands=*/true);

    m_audioComponent            = GetComponent<mnd::AudioComponent>();
    m_playerControllerComponent = GetComponent<mnd::PlayerControllerComponent>();

    RegisterHandsTweakerPanel();
}

void Player::RebuildIdleClip()
{
    if (!m_handsAnimComponent)
    {
        return;
    }
    auto skel = m_handsAnimComponent->GetSkeleton();
    if (!skel)
    {
        return;
    }
    auto clip = mnd::MakeBreathingIdleClip(*skel, m_idleDuration, m_idleAmpScale);
    m_handsAnimComponent->RegisterSkeletalClip(clip->name, clip);
    m_handsAnimComponent->Play(clip->name, true);
}

void Player::RegisterHandsTweakerPanel()
{
    auto &editor = mnd::Engine::GetInstance().GetEditor();
    editor.RegisterPanel(
        "Hands Tweaker",
        [this]()
        {
            ImGui::TextUnformatted("Sword view-model");
            ImGui::Separator();

            int mode = m_usingHands ? 0 : 1;
            ImGui::RadioButton("Sword (1)", &mode, 0);
            ImGui::SameLine();
            ImGui::RadioButton("Gun (2)", &mode, 1);
            SetViewmodel(mode == 0);

            ImGui::Separator();
            ImGui::TextUnformatted("Rest transform");

            bool dirtyXform = false;
            dirtyXform |= ImGui::DragFloat3("Position", &m_handsPosition.x, 0.005f, -3.0f, 3.0f, "%.3f");
            dirtyXform |= ImGui::DragFloat3("Rotation (deg)", &m_handsRotEuler.x, 0.5f, -180.0f, 180.0f, "%.2f");
            dirtyXform |= ImGui::DragFloat("Uniform scale", &m_handsScale, 0.0005f, 0.0001f, 5.0f, "%.5f");

            if (dirtyXform && m_handsObject)
            {
                m_swordRestPos = m_handsPosition;
                m_swordRestRot = glm::quat(glm::radians(m_handsRotEuler));
                m_handsObject->SetPosition(m_swordRestPos);
                m_handsObject->SetRotation(m_swordRestRot);
                m_handsObject->SetScale(mnd::vec3(m_handsScale));
            }

            if (ImGui::Button("Print JSON"))
            {
                LOG_INFO(
                "[Sword] paste into scene JSON:\n"
                "  \"position\": { \"x\": %.4f, \"y\": %.4f, \"z\": %.4f },\n"
                "  \"rotation_deg\": { \"x\": %.2f, \"y\": %.2f, \"z\": %.2f },\n"
                "  \"scale\": { \"x\": %.5f, \"y\": %.5f, \"z\": %.5f }",
                m_handsPosition.x, m_handsPosition.y, m_handsPosition.z,
                m_handsRotEuler.x, m_handsRotEuler.y, m_handsRotEuler.z,
                m_handsScale, m_handsScale, m_handsScale);
            }

            ImGui::Separator();
            ImGui::TextUnformatted("Combat — live state");

            ImGui::Text("Attacking: %s   t=%.2f / %.2f",
                        m_attacking ? "yes" : "no", m_attackTime, m_attackDuration);
            ImGui::Text("Blocking : %s   blend=%.2f",
                        m_blocking ? "yes" : "no", m_blockBlend);
            // Visual progress bars so it's easy to eyeball curve timing while tuning.
            f32 attackPct = m_attacking ? glm::clamp(m_attackTime / m_attackDuration, 0.0f, 1.0f) : 0.0f;
            ImGui::ProgressBar(attackPct, ImVec2(-1.0f, 0.0f), "attack");
            ImGui::ProgressBar(m_blockBlend, ImVec2(-1.0f, 0.0f), "block");

            if (ImGui::Button("Trigger attack"))
            {
                StartAttack();
            }

            if (ImGui::CollapsingHeader("Model clips", ImGuiTreeNodeFlags_DefaultOpen))
            {
                // Procedural anims live on the Player, not on the AnimationComponent —
                // expose them alongside the model's own clips so the panel reads as
                // a single, complete trigger list.
                ImGui::TextDisabled("Procedural (Player-driven)");
                if (ImGui::Button("attack##proc"))
                {
                    StartAttack();
                }
                ImGui::SameLine();
                bool blockToggle = m_blocking;
                if (ImGui::Checkbox("block (hold)##proc", &blockToggle))
                {
                    m_blocking = blockToggle;
                }

                // Walk every AnimationComponent reachable from the Player's children
                // and surface a "play" / "loop" / "stop" trio per registered clip.
                auto drawClipsFor = [](const char *label, mnd::AnimationComponent *anim)
                {
                    if (!anim)
                    {
                        return;
                    }
                    auto names = anim->GetClipNames();
                    ImGui::TextDisabled("%s — %zu clip%s",
                                        label, names.size(), names.size() == 1 ? "" : "s");
                    if (names.empty())
                    {
                        ImGui::SameLine();
                        ImGui::TextDisabled("(none registered)");
                        return;
                    }
                    for (const auto &name : names)
                    {
                        ImGui::PushID(name.c_str());
                        if (ImGui::Button("play"))
                        {
                            anim->Play(name, /*loop=*/false);
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("loop"))
                        {
                            anim->Play(name, /*loop=*/true);
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("stop"))
                        {
                            anim->Stop();
                        }
                        ImGui::SameLine();
                        ImGui::TextUnformatted(name.c_str());
                        ImGui::PopID();
                    }
                };

                drawClipsFor("Sword (hands)", m_handsAnimComponent);
                drawClipsFor("Gun", m_animationComponent);
            }

            if (ImGui::CollapsingHeader("Attack animation", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::SliderFloat("Duration (s)", &m_attackDuration, 0.1f, 1.5f, "%.2f");

                // Phase splits: keep windup < strike < 1.0 so the curve stays valid.
                ImGui::SliderFloat("Windup end (frac)", &m_attackWindupEnd, 0.05f, 0.9f, "%.2f");
                if (m_attackStrikeEnd <= m_attackWindupEnd + 0.01f)
                {
                    m_attackStrikeEnd = glm::min(m_attackWindupEnd + 0.05f, 0.99f);
                }
                ImGui::SliderFloat("Strike end (frac)", &m_attackStrikeEnd,
                                   m_attackWindupEnd + 0.01f, 0.99f, "%.2f");

                ImGui::TextDisabled("Windup waypoint (sword cocked back)");
                ImGui::DragFloat3("Pos##aw", &m_attackWindupPos.x, 0.01f, -2.0f, 2.0f, "%.2f");
                ImGui::DragFloat3("Rot deg##aw", &m_attackWindupRot.x, 1.0f, -180.0f, 180.0f, "%.1f");

                ImGui::TextDisabled("Strike waypoint (follow-through)");
                ImGui::DragFloat3("Pos##as", &m_attackStrikePos.x, 0.01f, -2.0f, 2.0f, "%.2f");
                ImGui::DragFloat3("Rot deg##as", &m_attackStrikeRot.x, 1.0f, -180.0f, 180.0f, "%.1f");

                if (ImGui::Button("Reset attack"))
                {
                    m_attackDuration  = 0.35f;
                    m_attackWindupEnd = 0.22f;
                    m_attackStrikeEnd = 0.55f;
                    m_attackWindupPos = mnd::vec3(0.18f, 0.35f, 0.20f);
                    m_attackWindupRot = mnd::vec3(-55.0f, 35.0f, -40.0f);
                    m_attackStrikePos = mnd::vec3(-0.35f, -0.45f, -0.65f);
                    m_attackStrikeRot = mnd::vec3(75.0f, -30.0f, 55.0f);
                }
                ImGui::SameLine();
                if (ImGui::Button("Mirror attack X"))
                {
                    m_attackWindupPos.x = -m_attackWindupPos.x;
                    m_attackStrikePos.x = -m_attackStrikePos.x;
                    m_attackWindupRot.y = -m_attackWindupRot.y;
                    m_attackWindupRot.z = -m_attackWindupRot.z;
                    m_attackStrikeRot.y = -m_attackStrikeRot.y;
                    m_attackStrikeRot.z = -m_attackStrikeRot.z;
                }
            }

            if (ImGui::CollapsingHeader("Block animation", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::SliderFloat("Blend rate (1/s)", &m_blockBlendRate, 1.0f, 40.0f, "%.1f");
                ImGui::DragFloat3("Pos offset", &m_blockPosOffset.x, 0.01f, -1.5f, 1.5f, "%.2f");
                ImGui::DragFloat3("Rot deg",    &m_blockRotDeg.x,    1.0f, -180.0f, 180.0f, "%.1f");

                if (ImGui::Button("Reset block"))
                {
                    m_blockBlendRate  = 12.0f;
                    m_blockPosOffset  = mnd::vec3(-0.20f, 0.20f, 0.10f);
                    m_blockRotDeg     = mnd::vec3(-25.0f, 75.0f, -55.0f);
                }
                ImGui::SameLine();
                if (ImGui::Button("Mirror block X"))
                {
                    m_blockPosOffset.x = -m_blockPosOffset.x;
                    m_blockRotDeg.y    = -m_blockRotDeg.y;
                    m_blockRotDeg.z    = -m_blockRotDeg.z;
                }
                ImGui::SameLine();
                if (ImGui::Button("Preview"))
                {
                    // Snap blend to 1 so the user can see the pose without holding RMB.
                    m_blockBlend = 1.0f;
                }
            }
        }
    );
}

void Player::SetViewmodel(bool useHands)
{
    m_usingHands = useHands;
    if (m_gunObject)
    {
        m_gunObject->SetActive(!useHands);
    }
    if (m_handsObject)
    {
        m_handsObject->SetActive(useHands);
    }
}

void Player::ApplySwordPose(const vec3 &posOffset, const vec3 &rotOffsetDeg)
{
    if (!m_handsObject)
    {
        return;
    }
    quat rotOffset = glm::quat(glm::radians(rotOffsetDeg));
    m_handsObject->SetPosition(m_swordRestPos + posOffset);
    m_handsObject->SetRotation(m_swordRestRot * rotOffset);
}

void Player::StartAttack()
{
    if (m_attacking)
    {
        return;
    }
    m_attacking  = true;
    m_attackTime = 0.0f;
    if (m_audioComponent)
    {
        if (m_audioComponent->IsPlaying(kSfxSwordSwing))
        {
            m_audioComponent->Stop(kSfxSwordSwing);
        }
        m_audioComponent->Play(kSfxSwordSwing);
    }
}

void Player::UpdateSwordAnimation(f32 dt)
{
    if (!m_handsObject)
    {
        return;
    }

    // Block weight follows held-state via exponential smoothing, so press/release
    // both have a short ease rather than a snap.
    f32 blockTarget = m_blocking ? 1.0f : 0.0f;
    f32 alpha       = 1.0f - std::exp(-m_blockBlendRate * dt);
    m_blockBlend    = m_blockBlend + (blockTarget - m_blockBlend) * alpha;

    // Attack curve: rise (windup) -> peak (strike) -> fall (recover). Triangular.
    f32 attackWeight = 0.0f;
    vec3 attackPos   = vec3(0.0f);
    vec3 attackRot   = vec3(0.0f);
    if (m_attacking)
    {
        m_attackTime += dt;
        f32 t = m_attackTime / m_attackDuration;
        if (t >= 1.0f)
        {
            m_attacking  = false;
            m_attackTime = 0.0f;
            // Play whoosh tail when the strike completes without a hit.
            if (m_audioComponent && !m_audioComponent->IsPlaying(kSfxSwordMiss))
            {
                m_audioComponent->Play(kSfxSwordMiss);
            }
        }
        else if (t < m_attackWindupEnd)
        {
            // Windup: ease-in so the blade settles back over the shoulder
            // before snapping forward.
            f32 k        = EaseInQuad(t / m_attackWindupEnd);
            attackWeight = k;
            attackPos    = m_attackWindupPos * k;
            attackRot    = m_attackWindupRot * k;
        }
        else if (t < m_attackStrikeEnd)
        {
            // Strike: ease-out cubic — fast initial acceleration into the cut.
            f32 k        = EaseOutCubic((t - m_attackWindupEnd) /
                                        std::max(0.0001f, m_attackStrikeEnd - m_attackWindupEnd));
            attackWeight = 1.0f;
            attackPos    = glm::mix(m_attackWindupPos, m_attackStrikePos, k);
            attackRot    = glm::mix(m_attackWindupRot, m_attackStrikeRot, k);
        }
        else
        {
            // Recover: ease-out so the sword settles softly back to rest.
            f32 k        = EaseOutCubic((t - m_attackStrikeEnd) /
                                        std::max(0.0001f, 1.0f - m_attackStrikeEnd));
            attackWeight = 1.0f - k;
            attackPos    = m_attackStrikePos * (1.0f - k);
            attackRot    = m_attackStrikeRot * (1.0f - k);
        }
    }

    // Block pose holds while RMB is down; attack swing layers additively on top
    // so a guarding player can still strike.
    vec3 finalPos = m_blockPosOffset * m_blockBlend + attackPos;
    vec3 finalRot = m_blockRotDeg * m_blockBlend + attackRot;
    ApplySwordPose(finalPos, finalRot);
}

void Player::Update(f32 deltaTime)
{
    auto &input = Engine::GetInstance().GetInputManager();

    if (input.IsKeyPressed(Key::Num1))
    {
        SetViewmodel(true);
    }
    if (input.IsKeyPressed(Key::Num2))
    {
        SetViewmodel(false);
    }

    if (m_usingHands)
    {
        // Sword combat: LMB swings, RMB raises guard.
        // Attack and block are independent — you can swing while guarding.
        if (input.IsMouseButtonPressed(MouseButton::Left) && !m_attacking)
        {
            StartAttack();
        }

        bool wasBlocking = m_blocking;
        m_blocking       = input.IsMouseButtonPressed(MouseButton::Right);
        if (m_blocking && !wasBlocking && m_audioComponent)
        {
            m_audioComponent->Play(kSfxSwordBlock);
        }

        UpdateSwordAnimation(deltaTime);
    }
    else
    {
        // Reset sword overlay state so a re-equip starts clean.
        m_attacking  = false;
        m_blocking   = false;
        m_blockBlend = 0.0f;

        if (input.IsMouseButtonPressed(MouseButton::Left))
        {
            if (m_animationComponent && !m_animationComponent->IsPlaying())
            {
                m_animationComponent->Play(kAnimShoot, false);

                if (m_audioComponent)
                {
                    if (m_audioComponent->IsPlaying(kSfxShoot))
                    {
                        m_audioComponent->Stop(kSfxShoot);
                    }
                    m_audioComponent->Play(kSfxShoot);
                }

                auto bullet   = GetScene()->CreateObject<Bullet>("bullet");
                bullet->SetShooter(this);
                auto material = mnd::Material::Load("materials/bullet.mat");
                auto mesh     = mnd::Mesh::CreateSphere(0.1f, 32, 32);

                bullet->AddComponent(new mnd::MeshComponent(material, mesh));

                vec3 pos = vec3(0.0f);
                if (auto child = FindChildByName(kChildBoomName))
                {
                    pos = child->GetWorldPosition();
                }
                bullet->SetPosition(pos + GetRotation() * vec3(-0.2f, 0.2f, -1.75f));

                auto collider  = std::make_shared<mnd::SphereCollider>(0.2f);
                auto rigidBody = std::make_shared<RigidBody>(mnd::BodyType::Dynamic, collider, 5.0f, 1.0f);
                bullet->AddComponent(new mnd::PhysicsComponent(rigidBody));

                // CCD: bullet moves ~1.67 u/frame at 60fps, way past sphere radius 0.2 — needs swept test.
                rigidBody->EnableCcd(0.1f, 0.15f);

                vec3 front = GetRotation() * vec3(0.0f, 0.0f, -1.0f);
                rigidBody->ApplyImpulse(front * 500.0f);
            }
        }
    }

    if (input.IsKeyPressed(Key::Space))
    {
        if (m_audioComponent && !m_audioComponent->IsPlaying(kSfxJump) && m_playerControllerComponent->OnGround())
        {
            m_audioComponent->Play(kSfxJump);
        }
    }

    // clang-format off
    bool walking =
        input.IsKeyPressed(Key::W) ||
        input.IsKeyPressed(Key::A) ||
        input.IsKeyPressed(Key::S) ||
        input.IsKeyPressed(Key::D);
    // clang-format on

    if (walking && m_playerControllerComponent && m_playerControllerComponent->OnGround())
    {
        if (m_audioComponent && !m_audioComponent->IsPlaying(kSfxStep))
        {
            m_audioComponent->Play(kSfxStep, true);
        }
    } else
    {
        if (m_audioComponent && m_audioComponent->IsPlaying(kSfxStep))
        {
            m_audioComponent->Stop(kSfxStep);
        }
    }

    mnd::GameObject::Update(deltaTime);
}
