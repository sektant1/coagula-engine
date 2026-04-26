#include "scene/Component.h"

namespace mnd
{

size_t Component::nextId = 1;

void Component::LoadProperties(const nlohmann::json &json) {}

void Component::Init() {}

GameObject *Component::GetOwner()
{
    return m_owner;
}

ComponentFactory &ComponentFactory::GetInstance()
{
    static ComponentFactory instance;
    return instance;
}

}  // namespace mnd
