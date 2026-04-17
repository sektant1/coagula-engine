#include "scene/Component.h"

namespace ENG
{

GameObject *Component::GetOwner()
{
    return m_owner;
}

}  // namespace ENG
