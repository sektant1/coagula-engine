#pragma once
#include "scene/Component.h"

namespace ENG
{

class LightComponent : public Component
{
    COMPONENT(LightComponent)
public:
    void        Update(f32 deltaTime) override;
    const vec3 &GetColor() const;
    void        SetColor(const vec3 &color);

private:
    vec3 m_color = vec3(1.0F);
};

}  // namespace ENG
