/**
 * @file Component.h
 * @ingroup coa_scene
 * @brief Abstract base for all GameObject components and the COMPONENT macro.
 *
 * ## Entity-Component pattern
 * A GameObject is a named scene node with a transform. Behaviour and
 * rendering are added by attaching Component subclasses. Each component
 * type gets a unique numeric ID at runtime so that typed lookup via
 * GameObject::GetComponent<T>() works in O(n) without RTTI.
 *
 * ## Adding a new component type
 * 1. Subclass Component.
 * 2. Place the COMPONENT(MyComponent) macro at the top of the public section.
 * 3. Implement Update(f32 deltaTime).
 * 4. Attach via GameObject::AddComponent(new MyComponent(...)).
 *
 * @code
 *   class SpinComponent : public COA::Component {
 *       COMPONENT(SpinComponent)
 *   public:
 *       void Update(COA::f32 dt) override { m_owner->SetRotation(...); }
 *   };
 * @endcode
 *
 * @see GameObject::AddComponent, GameObject::GetComponent, COMPONENT
 */

#pragma once

#include "Types.h"
#include "nlohmann/json.hpp"

namespace COA
{
class GameObject;

class Component
{
public:
    virtual ~Component() = default;
    virtual void   LoadProperties(const nlohmann::json &json);
    virtual void   Update(f32 deltaTime) = 0;
    virtual void   Init();
    virtual size_t GetTypeId() const = 0;

    GameObject *GetOwner();

    template<typename T>
    static size_t StaticTypeId()
    {
        static size_t typeId = nextId++;
        return typeId;
    }

protected:
    GameObject *m_owner = nullptr;

    friend class GameObject;

private:
    static size_t nextId;
};

class ComponentCreatorBase
{
public:
    virtual ~ComponentCreatorBase()      = default;
    virtual Component *CreateComponent() = 0;
};

template<typename T>
class ComponentCreator : public ComponentCreatorBase
{
public:
    Component *CreateComponent() override { return new T(); }
};

class ComponentFactory
{
public:
    static ComponentFactory &GetInstance();

    template<typename T>
    void RegisterComponent(const std::string &name)
    {
        m_creators.emplace(name, std::make_unique<ComponentCreator<T>>());
    }

    Component *CreateComponent(const std::string &name)
    {
        auto it = m_creators.find(name);
        if (it != m_creators.end())
        {
            return it->second->CreateComponent();
        }

        return nullptr;
    }

private:
    std::unordered_map<std::string, std::unique_ptr<ComponentCreatorBase>> m_creators;
};

#define COMPONENT(ComponentClass) \
public: \
    static size_t TypeId() \
    { \
        return COA::Component::StaticTypeId<ComponentClass>(); \
    } \
    size_t GetTypeId() const override \
    { \
        return TypeId(); \
    } \
    static void Register() \
    { \
        COA::ComponentFactory::GetInstance().RegisterComponent<ComponentClass>(std::string(#ComponentClass)); \
    }
}  // namespace COA
