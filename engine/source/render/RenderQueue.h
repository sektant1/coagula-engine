#pragma once

#include <vector>

#include "Common.h"
#include "Types.h"

namespace ENG
{
class Mesh;
class Material;
class GraphicsAPI;

struct RenderCommand
{
    Mesh     *mesh     = nullptr;
    Material *material = nullptr;
    mat4      modelMatrix;
};

class RenderQueue
{
public:
    void Submit(const RenderCommand &command);
    void Draw(GraphicsAPI &graphicsAPI, const CameraData &cameraData, const std::vector<LightData> &lights);

private:
    std::vector<RenderCommand> m_commands;
};
}  // namespace ENG
