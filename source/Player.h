#pragma once

#include "Monad.h"

namespace mnd
{
class AnimationComponent;
class AudioComponent;
class PlayerControllerComponent;
}  // namespace mnd

class Player : public mnd::GameObject
{
    GAMEOBJECT(Player)
 public:
    Player() = default;
    void Init();
    void Update(mnd::f32 deltaTime) override;

 private:
    void SetViewmodel(bool useHands);
    void RegisterHandsTweakerPanel();
    void RebuildIdleClip();
    void UpdateSwordAnimation(mnd::f32 dt);
    void StartAttack();
    void ApplySwordPose(const mnd::vec3 &posOffset, const mnd::vec3 &rotOffsetDeg);

    mnd::AnimationComponent        *m_animationComponent        = nullptr;  ///< Gun anim (legacy carbine).
    mnd::AnimationComponent        *m_handsAnimComponent        = nullptr;  ///< Arms idle/anim driver.
    mnd::AudioComponent            *m_audioComponent            = nullptr;
    mnd::PlayerControllerComponent *m_playerControllerComponent = nullptr;

    mnd::GameObject *m_gunObject   = nullptr;
    mnd::GameObject *m_handsObject = nullptr;

    bool m_usingHands = false;  ///< false = gun (default, matches legacy behavior).

    // F1 tweaker state — surfaced through the editor's "Hands Tweaker" panel.
    mnd::vec3 m_handsPosition = mnd::vec3(0.51f, -0.45f, -0.7f);
    mnd::vec3 m_handsRotEuler = mnd::vec3(
        -14.0f, -104.0f, 16.0f
    );                                 ///< Degrees, intrinsic XYZ — sword rest pose (blade up, slight forward tilt).
    mnd::f32 m_handsScale   = 0.045f;  ///< Uniform scale.
    mnd::f32 m_idleDuration = 4.0f;
    mnd::f32 m_idleAmpScale = 1.0f;

    // Sword rest pose (cached at Init from the tweaker fields above).
    mnd::vec3 m_swordRestPos = mnd::vec3(0.0f);
    mnd::quat m_swordRestRot = mnd::quat(1.0f, 0.0f, 0.0f, 0.0f);

    // Attack state — single-stroke timer-driven swing. Curve waypoints are
    // tweaker-tunable so a designer can dial in feel without rebuilding.
    bool      m_attacking       = false;
    mnd::f32  m_attackTime      = 0.0f;
    mnd::f32  m_attackDuration  = 0.35f;
    mnd::f32  m_attackWindupEnd = 0.22f;  ///< Phase split, fraction of duration.
    mnd::f32  m_attackStrikeEnd = 0.55f;
    mnd::vec3 m_attackWindupPos = mnd::vec3(0.18f, 0.35f, 0.20f);
    mnd::vec3 m_attackWindupRot = mnd::vec3(-55.0f, 35.0f, -40.0f);
    mnd::vec3 m_attackStrikePos = mnd::vec3(-0.35f, -0.45f, -0.65f);
    mnd::vec3 m_attackStrikeRot = mnd::vec3(75.0f, -30.0f, 55.0f);

    // Block state — held while RMB is down; smooth lerp in/out.
    bool      m_blocking       = false;
    mnd::f32  m_blockBlend     = 0.0f;   ///< [0,1], smoothed toward target.
    mnd::f32  m_blockBlendRate = 24.0f;  ///< 1/seconds — exponential approach.
    mnd::vec3 m_blockPosOffset = mnd::vec3(-0.29f, 0.09f, 0.18f);
    mnd::vec3 m_blockRotDeg    = mnd::vec3(-72.0f, 123.0f, -69.0f);
};
