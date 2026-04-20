#pragma once

#include "ENG.h"

class Game : public ENG::Application
{
public:
    bool Init() override;
    void Update(float deltaTime) override;
    void Destroy() override;

private:
    ENG::Scene      *m_scene          = nullptr;
    ENG::GameObject *m_mainCamera     = nullptr;
    ENG::GameObject *m_altCamera      = nullptr;
    bool             m_toggleKeyPrev  = false;
    ENG::GameObject *m_orbitParent    = nullptr;
    float            m_orbitAngle     = 0.0F;
    ENG::GameObject *m_planet         = nullptr;
};
