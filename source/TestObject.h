#pragma once

#include "Monad.h"

class TestObject : public mnd::GameObject
{
public:
    TestObject();

    void Update(mnd::f32 deltaTime) override;

private:
};
