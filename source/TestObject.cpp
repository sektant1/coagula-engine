#include "TestObject.h"

#include <memory>

#include <GLFW/glfw3.h>

#include "Engine.h"
#include "GameConstants.h"
#include "graphics/GraphicsAPI.h"
#include "render/Builder.h"
#include "render/Material.h"
#include "scene/components/MeshComponent.h"

TestObject::TestObject()
{
    LOG_INFO("TestObject constructing");

    mnd::FileReader vertShader(kLabVertShaderPath);
    mnd::FileReader fragShader(kLabFragShaderPath);
    std::string     vertexShaderSource   = vertShader.ReadToString();
    std::string     fragmentShaderSource = fragShader.ReadToString();

    if (vertexShaderSource.empty() || fragmentShaderSource.empty()) {
        LOG_ERROR("TestObject shader source is empty (vert=%zu frag=%zu bytes)",
                  vertexShaderSource.size(),
                  fragmentShaderSource.size());
    }

    auto &graphicsAPI   = mnd::Engine::GetInstance().GetGraphicsAPI();
    auto  shaderProgram = graphicsAPI.CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
    if (!shaderProgram) {
        LOG_ERROR("TestObject failed to create shader program");
    }

    auto material = std::make_shared<mnd::Material>();
    material->SetShaderProgram(shaderProgram);

    auto mesh = mnd::Builder::CreateCube(1.0F, 1.0F).buildMesh();

    AddComponent(new mnd::MeshComponent(material, mesh));

    LOG_INFO("TestObject constructed");
}

void TestObject::Update(mnd::f32 deltaTime)
{
    mnd::GameObject::Update(deltaTime);

#if 0
    auto position = GetPosition();

    auto &input = mnd::Engine::GetInstance().GetInputManager();
    if (input.IsKeyPressed(mnd::Key::W)) {
        position.y += 1.0F * deltaTime;
    }
    if (input.IsKeyPressed(mnd::Key::A)) {
        position.x -= 1.0F * deltaTime;
    }
    if (input.IsKeyPressed(mnd::Key::S)) {
        position.y -= 1.0F * deltaTime;
    }
    if (input.IsKeyPressed(mnd::Key::D)) {
        position.x += 1.0F * deltaTime;
    }
    SetPosition(position);
#endif
}
