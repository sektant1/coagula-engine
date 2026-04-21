#include "scene/components/LightComponent.h"

namespace ENG
{
void LightComponent::Update(f32 deltaTime) {}

void LightComponent::SetColor(const vec3 &color)
{
    m_color = color;
}

const vec3 &LightComponent::GetColor() const
{
    return m_color;
}

}  // namespace ENG
