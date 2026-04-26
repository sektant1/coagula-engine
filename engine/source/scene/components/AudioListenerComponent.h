/**
 * @file AudioListenerComponent.h
 * @ingroup mnd_components
 * @brief Component whose owner becomes the 3D audio listener.
 *
 * Attach to whichever GameObject represents the player's "ears"
 * (usually the same one carrying a CameraComponent). Each frame this
 * component forwards the owner's world position to mnd::AudioManager
 * so panning and distance attenuation track the player.
 *
 * @code
 *   auto *cam = scene.CreateGameObject("Camera");
 *   cam->AddComponent<mnd::CameraComponent>();
 *   cam->AddComponent<mnd::AudioListenerComponent>();
 * @endcode
 */

#pragma once
#include "scene/Component.h"

namespace mnd
{
/**
 * @ingroup mnd_components
 * @brief Marks its GameObject as the 3D audio listener.
 */
class AudioListenerComponent : public Component
{
    COMPONENT(AudioListenerComponent)
public:
    /// Push the owner's world position to AudioManager once per frame.
    void Update(float deltaTime) override;
};
}  // namespace mnd
