/**
 * @file PhysicsComponent.h
 * @ingroup mnd_components
 * @brief Attaches a @ref RigidBody to a GameObject and syncs its transform.
 */

#pragma once
#include "physics/RigidBody.h"
#include "scene/Component.h"

namespace mnd
{
/**
 * @ingroup mnd_components
 * @brief Component bridge between a @ref GameObject and the physics world.
 *
 * Reads `collider` and `body` blocks from its JSON definition in a scene file,
 * registers the resulting @ref RigidBody with the engine's @ref PhysicsManager,
 * and each frame copies the simulated transform back onto the owning GameObject.
 */
class PhysicsComponent : public Component
{
    COMPONENT(PhysicsComponent)
public:
    PhysicsComponent() = default;

    /// Construct from an already-built rigid body (skips JSON parsing).
    PhysicsComponent(const std::shared_ptr<RigidBody> &body);

    /// Parse `collider` + `body` sub-objects and build the @ref RigidBody.
    void LoadProperties(const nlohmann::json &json) override;

    /// Register the rigid body with the PhysicsManager.
    void Init() override;

    /// Copy the simulated pose onto the owning GameObject.
    void Update(float deltaTime) override;

    /// Replace the component's rigid body at runtime.
    void                              SetRigidBody(const std::shared_ptr<RigidBody> &body);
    const std::shared_ptr<RigidBody> &GetRigidBody();

private:
    std::shared_ptr<RigidBody> m_rigidBody;
};
}  // namespace mnd
