/**
 * @file PhysicsManager.h
 * @ingroup coa_physics
 * @brief Owns the Bullet dynamics world and steps it each frame.
 */

#pragma once

#include <memory>

class btBroadphaseInterface;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;

namespace COA
{
class RigidBody;

/**
 * @ingroup coa_physics
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

    /// Raw pointer to the underlying Bullet world (for advanced/internal use).
    btDiscreteDynamicsWorld *GetWorld();

private:
    std::unique_ptr<btBroadphaseInterface>               m_broadphase;
    std::unique_ptr<btDefaultCollisionConfiguration>     m_collisionConfig;
    std::unique_ptr<btCollisionDispatcher>               m_dispatcher;
    std::unique_ptr<btSequentialImpulseConstraintSolver> m_solver;
    std::unique_ptr<btDiscreteDynamicsWorld>             m_world;
};
}  // namespace COA

