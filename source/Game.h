#pragma once

#include "Monad.h"

class Game : public mnd::Application
{
 public:
    bool Init() override;
    void Update(float deltaTime) override;
    void Destroy() override;

    void RegisterTypes() override;

 private:
    std::shared_ptr<mnd::Scene> m_scene;
};
