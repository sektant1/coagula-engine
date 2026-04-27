#include "scene/components/HealthComponent.h"

#include <algorithm>

#include "scene/GameObject.h"

namespace mnd
{

void HealthComponent::Update(f32 /*deltaTime*/)
{
}

void HealthComponent::LoadProperties(const nlohmann::json &json)
{
    if (json.contains("max"))
    {
        m_max = json["max"].get<int>();
    }
    m_current = json.value("current", m_max);
    m_current = std::clamp(m_current, 0, m_max);

    m_invulnerable   = json.value("invulnerable", false);
    m_destroyOnDeath = json.value("destroyOnDeath", false);
}

void HealthComponent::SetMax(int hp)
{
    m_max     = std::max(1, hp);
    m_current = std::min(m_current, m_max);
}

void HealthComponent::SetCurrent(int hp)
{
    m_current = std::clamp(hp, 0, m_max);
}

void HealthComponent::Heal(int amount)
{
    if (IsDead() || amount <= 0)
    {
        return;
    }
    m_current = std::min(m_max, m_current + amount);
}

void HealthComponent::Revive()
{
    m_current = m_max;
}

void HealthComponent::ApplyDamage(const DamageInfo &info)
{
    if (m_invulnerable || IsDead() || info.amount <= 0.0F)
    {
        return;
    }

    const int dmg = std::max(1, static_cast<int>(info.amount));
    m_current     = std::max(0, m_current - dmg);

    if (m_onDamage)
    {
        m_onDamage(info);
    }

    if (m_current == 0)
    {
        if (m_onDeath)
        {
            m_onDeath(info);
        }
        if (m_destroyOnDeath && m_owner != nullptr)
        {
            m_owner->MarkForDestroy();
        }
    }
}

}  // namespace mnd
