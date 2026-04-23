#include "physics/Collider.h"

#include <btBulletCollisionCommon.h>

#include "Types.h"

namespace COA
{
Collider::~Collider()
{
    if (m_shape)
    {
        delete m_shape;
    }
}

btCollisionShape *Collider::GetShape()
{
    return m_shape;
}

BoxCollider::BoxCollider(const vec3 &extents)
{
    vec3 halfExtents = extents * 0.5f;
    m_shape          = new btBoxShape(btVector3(halfExtents.x, halfExtents.y, halfExtents.z));
}

SphereCollider::SphereCollider(float radius)
{
    m_shape = new btSphereShape(radius);
}

CapsuleCollider::CapsuleCollider(float radius, float height)
{
    m_shape = new btCapsuleShape(btScalar(radius), btScalar(height));
}
}  // namespace COA
