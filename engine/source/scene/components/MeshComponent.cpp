#include "scene/components/MeshComponent.h"

#include "Engine.h"
#include "Log.h"
#include "render/Material.h"
#include "render/Mesh.h"
#include "render/RenderQueue.h"
#include "scene/GameObject.h"

namespace COA
{

MeshComponent::MeshComponent(const std::shared_ptr<Material> &material, const std::shared_ptr<Mesh> &mesh)
    : m_material(material)
    , m_mesh(mesh)
{
}

void MeshComponent::Update(f32 deltaTime)
{
    if (!m_material || !m_mesh)
    {
        const char *ownerName = GetOwner() ? GetOwner()->GetName().c_str() : "<no-owner>";
        LOG_WARN("MeshComponent on '%s' missing %s%s%s",
                 ownerName,
                 m_material ? "" : "material",
                 (!m_material && !m_mesh) ? " and " : "",
                 m_mesh ? "" : "mesh");
        return;
    }

    RenderCommand command;
    command.material    = m_material.get();
    command.mesh        = m_mesh.get();
    command.modelMatrix = GetOwner()->GetWorldTransform();

    auto &renderQueue = Engine::GetInstance().GetRenderQueue();
    renderQueue.Submit(command);
}

void MeshComponent::SetMaterial(const std::shared_ptr<Material> &material)
{
    m_material = material;
}

void MeshComponent::SetMesh(const std::shared_ptr<Mesh> &mesh)
{
    m_mesh = mesh;
}

void MeshComponent::LoadProperties(const nlohmann::json &json)
{
    // Material
    if (json.contains("material"))
    {
        const std::string matPath  = json.value("material", "");
        auto              material = Material::Load(matPath);
        if (material)
        {
            SetMaterial(material);
        }
    }

    // Mesh
    if (json.contains("mesh"))
    {
        const auto       &mesh = json["mesh"];
        const std::string type = mesh.value("type", "box");
        if (type == "box")
        {
            vec3 extents(mesh.value("x", 1.0F), mesh.value("y", 1.0F), mesh.value("z", 1.0F));
            auto mesh = Mesh::CreateBox(extents);
            SetMesh(mesh);
        }
    }
}

}  // namespace COA
