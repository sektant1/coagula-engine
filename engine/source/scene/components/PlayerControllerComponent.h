#pragma once

#include "Types.h"
#include "scene/Component.h"

namespace ENG
{

class PlayerControllerComponent : public Component
{
    COMPONENT(PlayerControllerComponent)
public:
    void Update(f32 deltaTime) override;

private:
    f32 m_sensitivity = 1.5F;
    f32 m_moveSpeed   = 1.0F;
};

}  // namespace ENG
