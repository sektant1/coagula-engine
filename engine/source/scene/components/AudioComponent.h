/**
 * @file AudioComponent.h
 * @ingroup mnd_components
 * @brief GameObject component that plays audio clips by name.
 *
 * Attach to any mnd::GameObject to give it a small library of named
 * mnd::Audio clips. Each frame the component pushes the GameObject's
 * world position to every clip so 3D spatialisation works without
 * extra plumbing.
 *
 * @code
 *   auto *src = enemy->AddComponent<mnd::AudioComponent>();
 *   auto growl = mnd::Audio::Load("sfx/growl.wav");
 *   src->RegisterAudio("growl", growl);
 *   src->Play("growl", true);  // looping
 * @endcode
 */

#pragma once
#include "audio/Audio.h"
#include "scene/Component.h"

namespace mnd
{
/**
 * @ingroup mnd_components
 * @brief Plays one or more named audio clips, attached to a GameObject.
 */
class AudioComponent : public Component
{
    COMPONENT(AudioComponent)
public:
    /// Read clips and properties from a serialised scene JSON entry.
    void LoadProperties(const nlohmann::json &json) override;
    /// Push the owner's world position to every registered clip.
    void Update(float deltaTime) override;

    /// Add a clip under @p name; later calls reference it by that key.
    void RegisterAudio(const std::string &name, std::shared_ptr<Audio> &clip);
    /// Start the named clip; @p loop true loops forever.
    void Play(const std::string &name, bool loop = false);
    /// Stop the named clip if it is currently playing.
    void Stop(const std::string &name);
    /// True iff the named clip is currently playing.
    bool IsPlaying(const std::string &name);

private:
    std::unordered_map<std::string, std::shared_ptr<Audio>> m_clips;
};
}  // namespace mnd
