
#pragma once
#include <COA.h>

#include "scene/GameObject.h"
#include "scene/components/AudioComponent.h"

namespace COA
{

class AnimationComponent;

class Player : public GameObject
{
    GAMEOBJECT(Player)
public:
    Player() = default;
    void Init();
    void Update(f32 deltaTime) override;

private:
    AnimationComponent        *m_animationComponent        = nullptr;
    AudioComponent            *m_audioComponent            = nullptr;
    PlayerControllerComponent *m_playerControllerComponent = nullptr;
};
}  // namespace COA
