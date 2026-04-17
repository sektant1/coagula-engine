#pragma once

#include "ENG.h"

class TestObject : public ENG::GameObject
{
public:
    TestObject();

    void Update(ENG::f32 deltaTime) override;

private:
};
