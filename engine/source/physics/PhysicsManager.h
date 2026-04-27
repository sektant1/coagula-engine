/**
 * @file PhysicsManager.h
 * @ingroup mnd_physics
 * @brief Owns the Bullet dynamics world and steps it each frame.
 */

#pragma once

#include <memory>

#include "Types.h"

class btBroadphaseInterface;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;

namespace mnd
{
class RigidBody;
struct RayHit;

/**
 * @ingroup mnd_physics
 * @brief Owns the Bullet `btDiscreteDynamicsWorld` and all auxiliary collision
 *        structures (broadphase, dispatcher, solver).
 *
 * One instance lives on the Engine singleton. `PhysicsComponent` pushes its
 * `RigidBody` here via @ref AddRigidBody; `Engine::Run` calls @ref Update once
 * per frame to step the simulation.
 */
class PhysicsManager
{
public:
    PhysicsManager();
    ~PhysicsManager();

    /// Allocate broadphase, dispatcher, solver, and dynamics world. Call once.
    void Init();

    /// Step the simulation by @p deltaTime seconds.
    void Update(float deltaTime);

    /// Register a rigid body so it participates in simulation and collision.
    void AddRigidBody(RigidBody *body);

    /// Unregister a rigid body (e.g. when its owning component is destroyed).
    void RemoveRigidBody(RigidBody *body);

    /// Cast a ray from @p from to @p to and fill @p out with the closest hit.
    /// `out.object` is the GameObject behind the hit collision object, when one
    /// has been set via CollisionObject::SetOwner; otherwise null.
    /// @return true if any collider was hit.
    bool Raycast(const vec3 &from, const vec3 &to, RayHit &out);

    /// Raw pointer to the underlying Bullet world (for advanced/internal use).
    btDiscreteDynamicsWorld *GetWorld();

private:
    std::unique_ptr<btBroadphaseInterface>               m_broadphase;
    std::unique_ptr<btDefaultCollisionConfiguration>     m_collisionConfig;
    std::unique_ptr<btCollisionDispatcher>               m_dispatcher;
    std::unique_ptr<btSequentialImpulseConstraintSolver> m_solver;
    std::unique_ptr<btDiscreteDynamicsWorld>             m_world;
};
}  // namespace mnd

