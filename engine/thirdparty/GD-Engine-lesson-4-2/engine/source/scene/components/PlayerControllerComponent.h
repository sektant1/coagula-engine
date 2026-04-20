#pragma once 

#include "scene/Component.h"

namespace eng
{
    class PlayerControllerComponent : public Component
    {
        COMPONENT(PlayerControllerComponent)

    public:
        void Update(float deltaTime) override;

    private:
        float m_sensitivity = 1.5f;
        float m_moveSpeed = 3.0f;
    };
}