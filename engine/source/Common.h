#pragma once

#include "Types.h"
#include "render/Material.h"

namespace ENG
{

struct CameraData
{
    mat4 viewMatrix;
    mat4 projectionMatrix;
};

struct LightData
{
    vec3 color;
    vec3 position;
};
}  // namespace ENG
