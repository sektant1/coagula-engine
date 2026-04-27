#include "combat/DamageSystem.h"

#include "Monad.h"
#include "scene/components/HealthComponent.h"

void DamageSystem::Apply(const DamageEvent &evt)
{
    if (evt.victim == nullptr || evt.amount <= 0.0F)
    {
        return;
    }

    auto *health = evt.victim->GetComponent<mnd::HealthComponent>();
    if (health == nullptr)
    {
        return;
    }

    mnd::DamageInfo info;
    info.attacker  = evt.attacker;
    info.amount    = evt.amount;
    info.hitPoint  = evt.hitPoint;
    info.hitNormal = evt.hitNormal;

    health->ApplyDamage(info);
}
