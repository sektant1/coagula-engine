/**
 * @file DamageEvent.h
 * @brief Payload describing a single damage-application from one GameObject to another.
 */

#pragma once

#include "Types.h"

namespace mnd
{
class GameObject;
}

enum class DamageType
{
    Generic,
    Bullet,
    Melee,
    Magic,
    Explosion
};

struct DamageEvent
{
    mnd::GameObject *attacker  = nullptr;
    mnd::GameObject *victim    = nullptr;
    float            amount    = 0.0F;
    DamageType       type      = DamageType::Generic;
    mnd::vec3        hitPoint{0.0F};
    mnd::vec3        hitNormal{0.0F};
};
