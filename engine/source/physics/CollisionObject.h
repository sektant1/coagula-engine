#pragma once

#include <vector>

#include "Common.h"
#include "Types.h"

namespace mnd
{

class IContactListener;
class GameObject;

enum class CollisionObjectType
{
    RigidBody,
    KinematicCharacterController
};

class CollisionObject
{
 public:
    CollisionObjectType GetCollisionObjectType();

    void AddContactListener(IContactListener *listener);
    void RemoveContactListener(IContactListener *listener);

    /// Backref to the owning GameObject so raycast hits can resolve to scene
    /// objects without a separate map. Set by PhysicsComponent / KCC owner.
    void                      SetOwner(GameObject *owner) { m_owner = owner; }
    [[nodiscard]] GameObject *GetOwner() const { return m_owner; }

 protected:
    void DispatchContactEvent(CollisionObject *obj, const vec3 &pos, const vec3 &norm);

    CollisionObjectType             m_collisionObjType;
    std::vector<IContactListener *> m_contactListeners;
    GameObject                     *m_owner = nullptr;

    friend class PhysicsManager;
};

class IContactListener
{
 public:
    virtual void OnContact(CollisionObject *obj, const vec3 &pos, const vec3 &norm) = 0;
};

}  // namespace mnd
