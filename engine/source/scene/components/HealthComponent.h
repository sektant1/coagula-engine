/**
 * @file HealthComponent.h
 * @ingroup mnd_components
 * @brief Universal HP sink: damageable + killable game objects.
 *
 * Any GameObject that can take damage (player, goblins, breakables) carries
 * a HealthComponent. The DamageSystem looks for this component on the victim
 * GameObject when applying a DamageEvent. Death emits an OnDeath callback so
 * gameplay code (loot drops, ragdoll, score) can react without coupling to
 * the damage source.
 */

#pragma once

#include <functional>

#include "scene/Component.h"

namespace mnd
{

struct DamageInfo
{
    GameObject *attacker = nullptr;
    float       amount   = 0.0F;
    vec3        hitPoint{0.0F};
    vec3        hitNormal{0.0F};
};

class HealthComponent : public Component
{
    COMPONENT(HealthComponent)
 public:
    using DamageFn = std::function<void(const DamageInfo &)>;
    using DeathFn  = std::function<void(const DamageInfo &)>;

    void Update(f32 deltaTime) override;
    void LoadProperties(const nlohmann::json &json) override;

    /// Apply damage. Routed through DamageSystem::Apply, do not call directly.
    void ApplyDamage(const DamageInfo &info);

    [[nodiscard]] int  GetMax() const { return m_max; }
    [[nodiscard]] int  GetCurrent() const { return m_current; }
    [[nodiscard]] bool IsDead() const { return m_current <= 0; }
    [[nodiscard]] bool IsInvulnerable() const { return m_invulnerable; }

    void SetMax(int hp);
    void SetCurrent(int hp);
    void SetInvulnerable(bool inv) { m_invulnerable = inv; }
    void SetDestroyOnDeath(bool b) { m_destroyOnDeath = b; }

    /// Heal up to max. No-op if dead (use Revive for that).
    void Heal(int amount);

    /// Reset to full HP and clear dead state.
    void Revive();

    /// Subscribe to damage events. Listeners fire on every non-zero damage tick.
    void OnDamage(DamageFn fn) { m_onDamage = std::move(fn); }
    /// Subscribe to death event. Fires once per transition from alive→dead.
    void OnDeath(DeathFn fn) { m_onDeath = std::move(fn); }

 private:
    int      m_max            = 100;
    int      m_current        = 100;
    bool     m_invulnerable   = false;
    bool     m_destroyOnDeath = false;
    DamageFn m_onDamage;
    DeathFn  m_onDeath;
};

}  // namespace mnd
