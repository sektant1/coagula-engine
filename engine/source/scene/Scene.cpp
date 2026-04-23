#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include "scene/Scene.h"

#include "Common.h"
#include "Engine.h"
#include "Log.h"
#include "scene/GameObject.h"
#include "scene/components/AnimationComponent.h"
#include "scene/components/CameraComponent.h"
#include "scene/components/LightComponent.h"
#include "scene/components/MeshComponent.h"
#include "scene/components/PhysicsComponent.h"
#include "scene/components/PlayerControllerComponent.h"

namespace COA
{

void Scene::RegisterTypes()
{
    AnimationComponent::Register();
    CameraComponent::Register();
    LightComponent::Register();
    MeshComponent::Register();
    PhysicsComponent::Register();
    PlayerControllerComponent::Register();
}

void Scene::Update(f32 deltaTime)
{
    for (auto it = m_objects.begin(); it != m_objects.end();)
    {
        if ((*it)->IsAlive())
        {
            (*it)->Update(deltaTime);
            ++it;
        } else
        {
            it = m_objects.erase(it);
        }
    }
}

void Scene::SetMainCamera(GameObject *camera)
{
    m_mainCamera = camera;
}

GameObject *Scene::GetMainCamera()
{
    return m_mainCamera;
}

std::vector<LightData> Scene::CollectLight()
{
    std::vector<LightData> lights;
    for (auto &obj : m_objects)
    {
        CollectLightsRecursive(obj.get(), lights);
    }
    return lights;
}

void Scene::CollectLightsRecursive(GameObject *obj, std::vector<LightData> &out)
{
    if (auto light = obj->GetComponent<LightComponent>())
    {
        LightData data;
        data.color    = light->GetColor();
        data.position = obj->GetWorldPosition();
        out.push_back(data);
    }

    for (auto &child : obj->m_children)
    {
        CollectLightsRecursive(child.get(), out);
    }
}

void Scene::Clear()
{
    LOG_INFO("Scene::Clear (%zu root objects)", m_objects.size());
    m_objects.clear();
}

GameObject *Scene::CreateObject(const std::string &type, const std::string &name, GameObject *parent)
{
    GameObject *obj = GameObjectFactory::GetInstance().CreateGameObject(type);
    if (obj)
    {
        obj->SetName(name);
        obj->m_scene = this;
        obj->SetParent(parent);
    }
    return obj;
}

GameObject *Scene::CreateObject(const std::string &name, GameObject *parent)
{
    auto obj = new GameObject();
    obj->SetName(name);
    obj->m_scene = this;
    SetParent(obj, parent);
    LOG_INFO("Created GameObject '%s' (parent=%s)", name.c_str(), parent ? parent->GetName().c_str() : "<root>");
    return obj;
}

void Scene::LoadObject(const nlohmann::json &jsonObject, GameObject *parent)
{
    const str name = jsonObject.value("name", "Object");

    GameObject *gameObject = nullptr;

    if (jsonObject.contains("type"))
    {
        const std::string type = jsonObject.value("type", "");
        if (type == "gltf")
        {
            std::string path = jsonObject.value("path", "");
            gameObject       = GameObject::LoadGLTF(path, this);
            if (gameObject)
            {
                gameObject->SetParent(parent);
                gameObject->SetName(name);
            } else
            {
                LOG_ERROR("Scene::LoadObject failed to load GLTF '%s' for object '%s'", path.c_str(), name.c_str());
            }
        } else
        {
            gameObject = CreateObject(type, name, parent);
            if (!gameObject)
            {
                LOG_ERROR("Scene::LoadObject unknown GameObject type '%s' (name='%s')", type.c_str(), name.c_str());
            }
        }
    } else
    {
        gameObject = CreateObject(name, parent);
    }

    if (!gameObject)
    {
        return;
    }

    // Read transform
    if (jsonObject.contains("position"))
    {
        auto      posObj = jsonObject["position"];
        glm::vec3 pos;
        pos.x = posObj.value("x", 0.0f);
        pos.y = posObj.value("y", 0.0f);
        pos.z = posObj.value("z", 0.0f);
        gameObject->SetPosition(pos);
        // gameObject->LoadProperties(posObj);
    }

    if (jsonObject.contains("rotation"))
    {
        auto      rotObj = jsonObject["rotation"];
        glm::quat rot;
        rot.x = rotObj.value("x", 0.0f);
        rot.y = rotObj.value("y", 0.0f);
        rot.z = rotObj.value("z", 0.0f);
        rot.w = rotObj.value("w", 1.0f);
        gameObject->SetRotation(rot);
        // gameObject->LoadProperties(rotObj);
    }

    if (jsonObject.contains("scale"))
    {
        auto      scaleObj = jsonObject["scale"];
        glm::vec3 scale;
        scale.x = scaleObj.value("x", 1.0f);
        scale.y = scaleObj.value("y", 1.0f);
        scale.z = scaleObj.value("z", 1.0f);
        gameObject->SetScale(scale);
        // gameObject->LoadProperties(scaleObj);
    }

    gameObject->LoadProperties(jsonObject);

    if (jsonObject.contains("components") && jsonObject["components"].is_array())
    {
        const auto &components = jsonObject["components"];
        for (const auto &comp : components)
        {
            const std::string type      = comp.value("type", "");
            Component        *component = ComponentFactory::GetInstance().CreateComponent(type);
            if (component)
            {
                component->LoadProperties(comp);
                gameObject->AddComponent(component);
            } else
            {
                LOG_ERROR("Unknown component type '%s' on object '%s' (not registered in ComponentFactory)",
                          type.c_str(),
                          name.c_str());
            }
        }
    }

    if (jsonObject.contains("children") && jsonObject["children"].is_array())
    {
        const auto &children = jsonObject["children"];
        for (const auto &child : children)
        {
            LoadObject(child, gameObject);
        }
    }

    gameObject->Init();
}

std::shared_ptr<Scene> Scene::Load(const str &path)
{
    LOG_INFO("Scene::Load '%s'", path.c_str());
    const str contents = Engine::GetInstance().GetFileSystem().LoadAssetFileText(path);
    if (contents.empty())
    {
        LOG_ERROR("Scene::Load empty or missing file '%s'", path.c_str());
        return nullptr;
    }

    nlohmann::json json;
    try
    {
        json = nlohmann::json::parse(contents);
    } catch (const nlohmann::json::parse_error &e)
    {
        LOG_ERROR("Scene::Load JSON parse error in '%s': %s", path.c_str(), e.what());
        return nullptr;
    }
    if (json.empty())
    {
        LOG_ERROR("Scene::Load JSON empty in '%s'", path.c_str());
        return nullptr;
    }

    auto result = std::make_shared<Scene>();

    const str sceneName = json.value("name", "noname");
    if (json.contains("objects") && json["objects"].is_array())
    {
        const auto &objects = json["objects"];
        for (const auto &obj : objects)
        {
            result->LoadObject(obj, nullptr);
        }
    } else
    {
        LOG_WARN("Scene '%s' has no 'objects' array", sceneName.c_str());
    }

    if (json.contains("camera"))
    {
        str cameraObjName = json.value("camera", "");
        for (const auto &child : result->m_objects)
        {
            if (auto object = child->FindChildByName(cameraObjName))
            {
                result->SetMainCamera(object);
                break;
            }
        }
        if (!result->GetMainCamera())
        {
            LOG_ERROR("Scene '%s' camera target '%s' not found in loaded objects",
                      sceneName.c_str(),
                      cameraObjName.c_str());
        }
    } else
    {
        LOG_WARN("Scene '%s' has no 'camera' key — render will use identity matrices", sceneName.c_str());
    }

    LOG_INFO("Scene '%s' loaded (%zu root objects)", sceneName.c_str(), result->m_objects.size());
    return result;
}

bool Scene::SetParent(GameObject *obj, GameObject *parent)
{
    if (!obj)
    {
        LOG_ERROR("Scene::SetParent called with null object");
        return false;
    }
    bool result        = false;
    auto currentParent = obj->GetParent();

    if (parent == nullptr)
    {
        if (currentParent != nullptr)
        {
            auto it = std::find_if(currentParent->m_children.begin(),
                                   currentParent->m_children.end(),
                                   [obj](const std::unique_ptr<GameObject> &el) { return el.get() == obj; });

            if (it != currentParent->m_children.end())
            {
                m_objects.push_back(std::move(*it));
                obj->m_parent = nullptr;
                currentParent->m_children.erase(it);
                result = true;
            }
        }

        // No parent currently. This can be in 2 cases.
        // 1. The object is in the scene root.
        // 2. The object has been just created.
        else
        {
            auto it = std::find_if(m_objects.begin(),
                                   m_objects.end(),
                                   [obj](const std::unique_ptr<GameObject> &el) { return el.get() == obj; });

            if (it == m_objects.end())
            {
                std::unique_ptr<GameObject> objHolder(obj);
                m_objects.push_back(std::move(objHolder));

                result = true;
            }
        }
    }
    // We are trying to add it as a child of another object

    else
    {
        if (currentParent != nullptr)
        {
            auto it = std::find_if(m_objects.begin(),
                                   m_objects.end(),
                                   [obj](const std::unique_ptr<GameObject> &el) { return el.get() == obj; });

            if (it != currentParent->m_children.end())
            {
                bool found          = false;
                auto currentElement = parent;
                while (currentParent)
                {
                    if (currentElement == obj)
                    {
                        found = true;
                        break;
                    }
                    currentElement = currentElement->GetParent();
                }

                if (!found)
                {
                    parent->m_children.push_back(std::move(*it));
                    obj->m_parent = parent;
                    currentParent->m_children.erase(it);
                    result = true;
                }
            }
        }

        // No parent currently. This can be in 2 cases.
        // 1. The object is in the scene root.
        // 2. The object has been just created.
        else
        {
            auto it = std::find_if(m_objects.begin(),
                                   m_objects.end(),
                                   [obj](const std::unique_ptr<GameObject> &el) { return el.get() == obj; });

            // The object has been just created
            if (it == m_objects.end())
            {
                std::unique_ptr<GameObject> objHolder(obj);
                parent->m_children.push_back(std::move(objHolder));
                obj->m_parent = parent;

                result = true;
            } else
            {
                bool found          = false;
                auto currentElement = parent;
                while (currentParent)
                {
                    if (currentElement == obj)
                    {
                        found = true;
                        break;
                    }
                    currentElement = currentElement->GetParent();
                }
                if (!found)
                {
                    parent->m_children.push_back(std::move(*it));
                    obj->m_parent = parent;
                    m_objects.erase(it);

                    result = true;
                }
            }
        }
    }

    if (!result)
    {
        LOG_WARN("Scene::SetParent failed for object '%s' (target parent=%s)",
                 obj->GetName().c_str(),
                 parent ? parent->GetName().c_str() : "<root>");
    }

    return result;
}
}  // namespace COA
