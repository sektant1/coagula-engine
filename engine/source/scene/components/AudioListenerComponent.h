#pragma once
#include "scene/Component.h"

namespace mnd
{
class AudioListenerComponent : public Component
{
    COMPONENT(AudioListenerComponent)
public:
    void Update(float deltaTime) override;
};
}  // namespace mnd

