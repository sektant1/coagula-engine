#pragma once

#include "Monad.h"
#include "GameConstants.h"

class LabObject : public mnd::GameObject
{
public:
    LabObject();

    void Update(mnd::f32 deltaTime) override;

private:
    std::shared_ptr<mnd::Material> m_material;

    float m_time      = 0.0F;
    float m_timeScale = kLabDefaultTimeScale;
};
