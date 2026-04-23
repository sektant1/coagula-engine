#include <memory>

#include "Game.h"

#include "COA.h"
#include "GLFW/glfw3.h"
#include "TestObject.h"

bool Game::Init()
{
    LOG_INFO("Game::Init");

    auto &fs      = COA::Engine::GetInstance().GetFileSystem();
    auto  texture = COA::Texture::Load("brick.png");

    m_scene = new COA::Scene();
    COA::Engine::GetInstance().SetScene(m_scene);

    auto camera = m_scene->CreateObject("Camera");
    camera->AddComponent(new COA::CameraComponent());
    camera->SetPosition(glm::vec3(0.0f, 0.0f, 2.0f));
    camera->AddComponent(new COA::PlayerControllerComponent());

    m_scene->SetMainCamera(camera);

    m_scene->CreateObject<TestObject>("TestObject");

    auto material = COA::Material::Load("materials/brick.mat");
    auto mesh     = COA::Mesh::CreateCube();

    auto objectB = m_scene->CreateObject("ObjectB");
    objectB->AddComponent(new COA::MeshComponent(material, mesh));
    objectB->SetPosition(glm::vec3(0.0f, 2.0f, 2.0f));
    objectB->SetRotation(glm::vec3(0.0f, 2.0f, 0.0f));

    auto objectC = m_scene->CreateObject("ObjectC");
    objectC->AddComponent(new COA::MeshComponent(material, mesh));
    objectC->SetPosition(glm::vec3(-2.0f, 0.0f, 0.0f));
    objectC->SetRotation(glm::vec3(1.0f, 0.0f, 1.0f));
    objectC->SetScale(glm::vec3(1.5f, 1.5f, 1.5f));

    auto suzanneObject = COA::GameObject::LoadGLTF("models/suzanne/Suzanne.gltf");
    suzanneObject->SetPosition(glm::vec3(0.0f, 0.0f, -5.0f));

    auto gun = COA::GameObject::LoadGLTF("models/carbine/scene.gltf");
    gun->SetParent(camera);
    gun->SetPosition(glm::vec3(0.75f, -0.5f, -0.75f));
    gun->SetScale(glm::vec3(-1.0f, 1.0f, 1.0f));

    if (auto anim = gun->GetComponent<COA::AnimationComponent>())
    {
        if (auto bullet = gun->FindChildByName("bullet_33"))
        {
            bullet->SetActive(false);
        }

        if (auto fire = gun->FindChildByName("BOOM_35"))
        {
            fire->SetActive(false);
        }

        anim->Play("shoot", false);
    }

    auto light     = m_scene->CreateObject("Light");
    auto lightComp = new COA::LightComponent();
    lightComp->SetColor(glm::vec3(1.0f));
    light->AddComponent(lightComp);
    light->SetPosition(glm::vec3(0.0f, 5.0f, 0.0f));

    auto ground = m_scene->CreateObject("Ground");
    ground->SetPosition(glm::vec3(0.0f, -5.0f, 0.0f));

    glm::vec3 groundExtents(20.0f, 2.0f, 20.0f);
    auto      groundMesh = COA::Mesh::CreateBox(groundExtents);
    ground->AddComponent(new COA::MeshComponent(material, groundMesh));

    auto groundCollider = std::make_shared<COA::BoxCollider>(groundExtents);
    auto groundBody     = std::make_shared<COA::RigidBody>(COA::BodyType::Static, groundCollider, 0.0f, 0.5f);
    ground->AddComponent(new COA::PhysicsComponent(groundBody));

    auto boxObj = m_scene->CreateObject("FallingBox");
    boxObj->AddComponent(new COA::MeshComponent(material, mesh));
    boxObj->SetPosition(glm::vec3(0.0f, 2.0f, 2.0f));
    boxObj->SetRotation(glm::quat(glm::vec3(1.0f, 2.0f, 0.0f)));
    auto boxCollider = std::make_shared<COA::BoxCollider>(glm::vec3(1.0f));
    auto boxBody     = std::make_shared<COA::RigidBody>(COA::BodyType::Dynamic, boxCollider, 5.0f, 0.5f);
    boxObj->AddComponent(new COA::PhysicsComponent(boxBody));

    camera->SetPosition(glm::vec3(0.0f, 1.0f, 7.0f));

    return true;
}

void Game::Update(float deltaTime)
{
    m_scene->Update(deltaTime);

    auto &inputManager = COA::Engine::GetInstance().GetInputManager();

    bool toggleKeyNow = inputManager.IsKeyPressed(GLFW_KEY_C);
    if (toggleKeyNow && !m_toggleKeyPrev)
    {
        auto *current = m_scene->GetMainCamera();
        m_scene->SetMainCamera(current == m_mainCamera ? m_altCamera : m_mainCamera);
    }
    m_toggleKeyPrev = toggleKeyNow;
}

void Game::Destroy()
{
    LOG_INFO("Game::Destroy");
}
