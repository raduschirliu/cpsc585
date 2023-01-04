#pragma once

#include <glm/glm.hpp>

struct Rect2
{
    Rect2(float x1, float y1, float x2, float y2)
        : x1(x1),
          y1(y1),
          x2(x2),
          y2(y2)
    {
    }

    bool IsOverlapping(const Rect2& other) const
    {
        // Bounds checking based on
        // https://stackoverflow.com/questions/306316/determine-if-two-rectangles-overlap-each-other
        return x1 < other.x2 && x2 > other.x1 && y1 > other.y2 && y2 < other.y1;
    }

    // Left
    float x1;

    // Top
    float y1;

    // Right
    float x2;

    // Bottom
    float y2;
};