#include "render/RenderQueue.h"

#include "graphics/GraphicsAPI.h"
#include "graphics/ShaderProgram.h"
#include "render/Material.h"
#include "render/Mesh.h"

namespace ENG
{

void RenderQueue::Submit(const RenderCommand &command)
{
    m_commands.push_back(command);
}

void RenderQueue::Draw(GraphicsAPI &graphicsAPI)
{
    for (auto &command : m_commands) {
        graphicsAPI.BindMaterial(command.material);
        command.material->GetShaderProgram()->SetUniform("uModel", command.modelMatrix);
        graphicsAPI.BindMesh(command.mesh);
        graphicsAPI.DrawMesh(command.mesh);
    }

    m_commands.clear();
}

}  // namespace ENG
