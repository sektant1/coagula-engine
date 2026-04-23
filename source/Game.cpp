#include <memory>

#include "Game.h"

#include "COA.h"
#include "GLFW/glfw3.h"
#include "Player.h"

bool Game::Init()
{
    LOG_INFO("Game::Init");

    auto scene = COA::Scene::Load("scenes/scene.json");
    m_scene    = scene;
    COA::Engine::GetInstance().SetScene(scene.get());

    return true;
}

void Game::RegisterTypes()
{
    COA::Player::Register();
}

void Game::Update(COA::f32 deltaTime)
{
    m_scene->Update(deltaTime);
    auto &inputManager = COA::Engine::GetInstance().GetInputManager();
    if (inputManager.IsKeyPressed(GLFW_KEY_ESCAPE))
    {
        SetNeedsToBeClosed(true);
    }
}

void Game::Destroy()
{
    LOG_INFO("Game::Destroy");
}
