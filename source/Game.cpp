#include "Game.h"

#include <GLFW/glfw3.h>

#include "Monad.h"
#include "GameConstants.h"
#include "Player.h"

bool Game::Init()
{
    LOG_INFO("Game::Init");

    auto scene = mnd::Scene::Load(kInitialScenePath);
    m_scene    = scene;
    mnd::Engine::GetInstance().SetScene(scene.get());

    return true;
}

void Game::RegisterTypes()
{
    Player::Register();
}

void Game::Update(mnd::f32 deltaTime)
{
    m_scene->Update(deltaTime);
    auto &inputManager = mnd::Engine::GetInstance().GetInputManager();
    if (inputManager.IsKeyPressed(mnd::Key::Escape))
    {
        SetNeedsToBeClosed(true);
    }
}

void Game::Destroy()
{
    LOG_INFO("Game::Destroy");
}
