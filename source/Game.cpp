#include "Game.h"

#include <memory>

#include "Engine.h"
#include "LabObject.h"
#include "TestObject.h"
#include "render/Builder.h"
#include "render/Material.h"
#include "scene/Scene.h"
#include "scene/components/CameraComponent.h"
#include "scene/components/MeshComponent.h"
#include "scene/components/PlayerControllerComponent.h"

bool Game::Init()
{
    LOG_INFO("Game::Init");

    m_scene = new ENG::Scene();

    auto camera = m_scene->CreateObject("Camera");
    camera->AddComponent(new ENG::CameraComponent());
    camera->SetPosition(ENG::vec3(0.0F, 0.0F, 2.0F));
    camera->AddComponent(new ENG::PlayerControllerComponent());

    m_scene->SetMainCamera(camera);

    m_scene->CreateObject<TestObject>("TestObject");

    ENG::FileReader vertShader("assets/shaders/lab.vert");
    ENG::FileReader fragShader("assets/shaders/lab.frag");
    std::string     vertexShaderSource   = vertShader.ReadToString();
    std::string     fragmentShaderSource = fragShader.ReadToString();

    if (vertexShaderSource.empty() || fragmentShaderSource.empty()) {
        LOG_ERROR("Game shader source is empty (vert=%zu frag=%zu bytes)",
                  vertexShaderSource.size(),
                  fragmentShaderSource.size());
    }

    auto &graphicsAPI   = ENG::Engine::GetInstance().GetGraphicsAPI();
    auto  shaderProgram = graphicsAPI.CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
    if (!shaderProgram) {
        LOG_ERROR("Game failed to create shader program");
    }

    auto material = std::make_shared<ENG::Material>();
    material->SetShaderProgram(shaderProgram);

    // Build cube once -> single GPU upload. Shared across all instances below.
    auto mesh = ENG::Builder::CreateCube(1.0F, 1.0F).buildMesh();

    auto objectA = m_scene->CreateObject("ObjectA");
    objectA->AddComponent(new ENG::MeshComponent(material, mesh));
    objectA->SetPosition(ENG::vec3(0.0F, 2.0F, 0.0F));

    auto objectB = m_scene->CreateObject("ObjectB");
    objectB->AddComponent(new ENG::MeshComponent(material, mesh));
    objectB->SetPosition(ENG::vec3(0.0F, 2.0F, 2.0F));
    objectB->SetRotation(ENG::vec3(0.0F, 2.0F, 0.0F));

    auto objectC = m_scene->CreateObject("ObjectC");
    objectC->AddComponent(new ENG::MeshComponent(material, mesh));
    objectC->SetPosition(ENG::vec3(-2.0F, 0.0F, 0.0F));
    objectC->SetRotation(ENG::vec3(1.0F, 0.0F, 1.0F));
    objectC->SetScale(ENG::vec3(1.5F, 1.5F, 1.5F));

    ENG::Engine::GetInstance().SetScene(m_scene);
    // m_scene->CreateObject<LabObject>("LabObject");
    return true;
}

void Game::Update(float deltaTime)
{
    m_scene->Update(deltaTime);
}

void Game::Destroy()
{
    LOG_INFO("Game::Destroy");
}
