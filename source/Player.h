#pragma once

#include "Monad.h"

namespace mnd
{
class AnimationComponent;
class AudioComponent;
class PlayerControllerComponent;
}  // namespace mnd

class Player : public mnd::GameObject
{
    GAMEOBJECT(Player)
public:
    Player() = default;
    void Init();
    void Update(mnd::f32 deltaTime) override;

private:
    mnd::AnimationComponent        *m_animationComponent        = nullptr;
    mnd::AudioComponent            *m_audioComponent            = nullptr;
    mnd::PlayerControllerComponent *m_playerControllerComponent = nullptr;
};
