/**
 * @file AudioManager.h
 * @ingroup mnd_audio
 * @brief Owns the global miniaudio engine and 3D listener position.
 *
 * One instance lives inside mnd::Engine; reach it with
 * `Engine::GetInstance().GetAudio()`. Every mnd::Audio clip plays
 * through the underlying `ma_engine` it returns. Update the listener
 * once per frame (typically from mnd::AudioListenerComponent::Update)
 * so 3D-positioned sounds pan and attenuate correctly.
 */

#pragma once

#include <memory>

#include "Common.h"
#include "Types.h"

struct ma_engine;

namespace mnd
{
/**
 * @ingroup mnd_audio
 * @brief Global audio backend wrapper.
 */
class AudioManager
{
public:
    AudioManager();
    ~AudioManager();

    /**
     * @brief Initialise the underlying miniaudio engine.
     * @return true if the audio device opened successfully.
     */
    bool Init();

    /// Underlying miniaudio engine handle (passed to clip Play()).
    ma_engine *GetEngine();

    /// Move the 3D listener; affects panning/attenuation of every clip.
    void SetListenerPosition(const vec3 &pos);

private:
    std::unique_ptr<ma_engine> m_engine;
};
}  // namespace mnd
