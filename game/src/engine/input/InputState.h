#pragma once

#include <array>
#include <cstdint>
#include <glm/glm.hpp>

enum class ButtonState : uint8_t
{
    kReleased = 0,
    kUp,
    kPressed,
    kDown
};

struct MouseState
{
    glm::ivec2 pos;
    std::array<ButtonState, GLFW_MOUSE_BUTTON_LAST> button_states;

    MouseState() : pos(0, 0), button_states{}
    {
    }

    void SetPos(int x, int y)
    {
        pos.x = x;
        pos.y = y;
    }
};