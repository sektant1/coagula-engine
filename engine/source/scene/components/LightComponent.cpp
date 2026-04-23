#include "scene/components/LightComponent.h"

namespace COA
{
void LightComponent::Update(f32 deltaTime) {}

void LightComponent::SetColor(const vec3 &color)
{
    m_color = color;
}

void LightComponent::LoadProperties(const nlohmann::json &json)
{
    if (json.contains("color"))
    {
        auto      c = json["color"];
        glm::vec3 color(c.value("r", 1.0f), c.value("g", 1.0f), c.value("b", 1.0f));
        SetColor(color);
    }
}

const vec3 &LightComponent::GetColor() const
{
    return m_color;
}

}  // namespace COA
