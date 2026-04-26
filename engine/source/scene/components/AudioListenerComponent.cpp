#include "scene/components/AudioListenerComponent.h"

#include "Engine.h"
#include "scene/GameObject.h"

namespace mnd
{
void AudioListenerComponent::Update(float deltaTime)
{
    auto pos = m_owner->GetWorldPosition();
    Engine::GetInstance().GetAudioManager().SetListenerPosition(pos);
}
}  // namespace mnd

