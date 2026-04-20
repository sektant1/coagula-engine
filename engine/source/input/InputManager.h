#pragma once
#include <array>

#include "Types.h"

namespace ENG
{

class InputManager
{
public:
    InputManager()                     = default;
    InputManager(const InputManager &) = delete;
    InputManager(InputManager &&)      = delete;

    InputManager &operator=(const InputManager &) = delete;
    InputManager &operator=(InputManager &&)      = delete;

    void SetKeyPressed(int key, bool pressed);
    bool IsKeyPressed(int key);

    void SetMouseButtonPressed(int button, bool pressed);
    bool IsMouseButtonPressed(int button);

    void       SetMousePositionOld(const vec2 &pos);
    const vec2 GetMousePositionOld() const;

    void       SetMousePositionCurrent(const vec2 &pos);
    const vec2 GetMousePositionCurrent() const;

private:
    std::array<bool, 256> m_keys                 = {false};
    std::array<bool, 16>  m_mouseKeys            = {false};
    vec2                  m_mousePositionOld     = vec2(0.0F);
    vec2                  m_mousePositionCurrent = vec2(0.0F);

    friend class Engine;
};

}  // namespace ENG
