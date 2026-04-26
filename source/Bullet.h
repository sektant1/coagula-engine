#pragma once

#include "Monad.h"
#include "GameConstants.h"

class Bullet : public mnd::GameObject
{
    GAMEOBJECT(Bullet)

public:
    void Update(mnd::f32 deltaTime) override;

private:
    mnd::f32 m_lifetime = kBulletLifetime;
};
