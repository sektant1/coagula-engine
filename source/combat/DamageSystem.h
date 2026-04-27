/**
 * @file DamageSystem.h
 * @brief Single chokepoint for applying damage. Resolves the victim's HealthComponent.
 *
 * All damage in the game flows through DamageSystem::Apply. This is the
 * single place to add armor, resistance, friendly-fire rules, score
 * reporting, or hit-marker FX without coupling damage sources to victims.
 *
 * Direct call (no event bus): the chokepoint is a function, not a queue.
 * Hooks (OnDamage / OnDeath) are subscribed per-victim via HealthComponent.
 */

#pragma once

#include "combat/DamageEvent.h"

class DamageSystem
{
 public:
    /// Apply @p evt to its victim if it has a HealthComponent. Idempotent for
    /// dead/invulnerable victims (safe no-op).
    static void Apply(const DamageEvent &evt);
};
