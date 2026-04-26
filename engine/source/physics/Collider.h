/**
 * @file Collider.h
 * @ingroup mnd_physics
 * @brief Thin RAII wrappers around Bullet collision shapes.
 */

#pragma once

#include <glm/vec3.hpp>

class btCollisionShape;

namespace mnd
{
/**
 * @ingroup mnd_physics
 * @brief Abstract owner of a `btCollisionShape`.
 *
 * Subclass constructors allocate a concrete Bullet shape; the destructor
 * releases it. Attach to a @ref RigidBody to give it geometry.
 */
class Collider
{
public:
    virtual ~Collider();

    /// Underlying Bullet shape (borrowed, do not delete).
    btCollisionShape *GetShape();

protected:
    btCollisionShape *m_shape = nullptr;
};

/// @ingroup mnd_physics @brief Axis-aligned box collider; @p extents are *half*-sizes.
class BoxCollider : public Collider
{
public:
    BoxCollider(const glm::vec3 &extents);
};

/// @ingroup mnd_physics @brief Sphere collider of the given @p radius.
class SphereCollider : public Collider
{
public:
    SphereCollider(float radius);
};

/// @ingroup mnd_physics @brief Y-axis capsule collider (cylinder + hemispherical caps).
class CapsuleCollider : public Collider
{
public:
    CapsuleCollider(float radius, float height);
};
}  // namespace mnd

