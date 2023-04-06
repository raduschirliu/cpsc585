#pragma once

#include <glm/glm.hpp>

struct Rect2d
{
    glm::vec2 pos;
    glm::vec2 size;

    constexpr Rect2d() : pos(0.0f, 0.0f), size(0.0f, 0.0f)
    {
    }

    constexpr Rect2d(const glm::vec2& pos) : pos(pos), size(0.0f, 0.0f)
    {
    }

    constexpr Rect2d(const glm::vec2& pos, const glm::vec2& size)
        : pos(pos),
          size(size)
    {
    }
};
