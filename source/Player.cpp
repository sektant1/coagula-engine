
#include "Player.h"

#include <GLFW/glfw3.h>

#include "scene/components/AudioComponent.h"
#include "scene/components/PlayerControllerComponent.h"

namespace COA
{

void Player::Init()
{
    if (auto bullet = FindChildByName("bullet_33"))
    {
        bullet->SetActive(false);
    }

    if (auto fire = FindChildByName("BOOM_35"))
    {
        fire->SetActive(false);
    }

    if (auto gun = FindChildByName("Gun"))
    {
        m_animationComponent = gun->GetComponent<COA::AnimationComponent>();
    }

    m_audioComponent            = GetComponent<COA::AudioComponent>();
    m_playerControllerComponent = GetComponent<COA::PlayerControllerComponent>();
}

void Player::Update(f32 deltaTime)
{
    auto &input = Engine::GetInstance().GetInputManager();
    if (input.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
    {
        if (m_animationComponent && !m_animationComponent->IsPlaying())
        {
            m_animationComponent->Play("shoot", false);

            if (m_audioComponent)
            {
                if (m_audioComponent->IsPlaying("shoot"))
                {
                    m_audioComponent->Stop("shoot");
                }
                m_audioComponent->Play("shoot");
            }
        }
    }

    if (input.IsKeyPressed(GLFW_KEY_SPACE))
    {
        if (m_audioComponent && !m_audioComponent->IsPlaying("jump") && m_playerControllerComponent->OnGround())
        {
            m_audioComponent->Play("jump");
        }
    }

    // clang-format off
    bool walking = 
        input.IsKeyPressed(GLFW_KEY_W) || 
        input.IsKeyPressed(GLFW_KEY_A) || 
        input.IsKeyPressed(GLFW_KEY_S) || 
        input.IsKeyPressed(GLFW_KEY_D);
    // clang-format on

    if (walking && m_playerControllerComponent && m_playerControllerComponent->OnGround())
    {
        if (m_audioComponent && !m_audioComponent->IsPlaying("step"))
        {
            m_audioComponent->Play("step", true);
        }
    } else
    {
        if (m_audioComponent && m_audioComponent->IsPlaying("step"))
        {
            m_audioComponent->Stop("step");
        }
    }
    if (input.IsKeyPressed(GLFW_KEY_LEFT_SHIFT) && walking)
    {
        m_playerControllerComponent->SetMS(10.0f);
    } else
    {
        m_playerControllerComponent->SetMS(5.0f);
    }

    GameObject::Update(deltaTime);
}

}  // namespace COA
