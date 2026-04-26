#include "Bullet.h"

void Bullet::Update(mnd::f32 deltaTime)
{
    mnd::GameObject::Update(deltaTime);
    m_lifetime -= deltaTime;
    if (m_lifetime <= 0.0F)
    {
        MarkForDestroy();
    }
}
