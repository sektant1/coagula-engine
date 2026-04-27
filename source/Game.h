#pragma once

#include "Monad.h"
#include "run/Rng.h"

class Game : public mnd::Application
{
 public:
    bool Init() override;
    void Update(float deltaTime) override;
    void Destroy() override;

    void RegisterTypes() override;

 private:
    void RegisterDebugPanels();

    std::shared_ptr<mnd::Scene> m_scene;
    mnd::RngStreams             m_rng;
};
