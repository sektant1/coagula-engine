#pragma once

#include "COA.h"

class Game : public COA::Application
{
public:
    bool Init() override;
    void Update(float deltaTime) override;
    void Destroy() override;

    void RegisterTypes() override;

private:
    std::shared_ptr<COA::Scene> m_scene;
};
