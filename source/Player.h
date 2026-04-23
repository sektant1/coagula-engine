
#pragma once
#include <COA.h>

#include "scene/GameObject.h"

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
    AnimationComponent *m_animationComponent = nullptr;
};
}  // namespace COA
