#include "scene/GameObject.h"
#include <glm/gtc/matrix_transform.hpp>

namespace eng
{
    void GameObject::Update(float deltaTime)
    {
        for (auto& component : m_components)
        {
            component->Update(deltaTime);
        }

        for (auto it = m_children.begin(); it != m_children.end();)
        {
            if ((*it)->IsAlive())
            {
                (*it)->Update(deltaTime);
                ++it;
            }
            else
            {
                it = m_children.erase(it);
            }
        }
    }

    const std::string& GameObject::GetName() const
    {
        return m_name;
    }

    void GameObject::SetName(const std::string& name)
    {
        m_name = name;
    }

    GameObject* GameObject::GetParent()
    {
        return m_parent;
    }

    bool GameObject::IsAlive() const
    {
        return m_isAlive;
    }

    void GameObject::MarkForDestroy()
    {
        m_isAlive = false;
    }

    void GameObject::AddComponent(Component* component)
    {
        m_components.emplace_back(component);
        component->m_owner = this;
    }

    const glm::vec3& GameObject::GetPosition() const
    {
        return m_position;
    }

    void GameObject::SetPosition(const glm::vec3& pos)
    {
        m_position = pos;
    }

    const glm::quat& GameObject::GetRotation() const
    {
        return m_rotation;
    }

    void GameObject::SetRotation(const glm::quat& rot)
    {
        m_rotation = rot;
    }

    const glm::vec3& GameObject::GetScale() const
    {
        return m_scale;
    }

    void GameObject::SetScale(const glm::vec3& scale)
    {
        m_scale = scale;
    }

    glm::mat4 GameObject::GetLocalTransform() const
    {
        glm::mat4 mat = glm::mat4(1.0f);

        // Translation
        mat = glm::translate(mat, m_position);

        // Rotation
        mat = mat * glm::mat4_cast(m_rotation);

        // Scale
        mat = glm::scale(mat, m_scale);

        return mat;
    }

    glm::mat4 GameObject::GetWorldTransform() const
    {
        if (m_parent)
        {
            return m_parent->GetWorldTransform() * GetLocalTransform();
        }
        else
        {
            return GetLocalTransform();
        }
    }
}