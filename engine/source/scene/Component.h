#pragma once

#include "Types.h"

namespace ENG
{

class GameObject;

class Component
{
public:
    virtual ~Component()               = default;
    virtual void Update(f32 deltaTime) = 0;

    GameObject *GetOwner();

protected:
    GameObject *m_owner = nullptr;

    friend class GameObject;
};
}  // namespace ENG
