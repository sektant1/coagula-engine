/**
 * @file Raycast.h
 * @ingroup mnd_physics
 * @brief Result struct for PhysicsManager::Raycast.
 */

#pragma once

#include "Types.h"

namespace mnd
{
class GameObject;

/**
 * @ingroup mnd_physics
 * @brief Hit information returned by PhysicsManager::Raycast.
 *
 * `object` may be null even when the ray hits — only collision objects whose
 * owning component called CollisionObject::SetOwner expose a back-pointer.
 */
struct RayHit
{
    vec3        point{0.0F};   ///< World-space hit point.
    vec3        normal{0.0F};  ///< Surface normal at the hit.
    GameObject *object   = nullptr;
    float       fraction = 1.0F;  ///< Fraction along the ray segment in [0, 1].
};
}  // namespace mnd
