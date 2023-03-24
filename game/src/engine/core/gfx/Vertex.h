#pragma once

#include <glm/glm.hpp>

#include "engine/core/Colors.h"

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;

    explicit constexpr Vertex(glm::vec3 position)
        : position(position),
          normal(0.0f, 0.0f, 0.0f),
          uv(0.0f, 0.0f)
    {
    }

    constexpr Vertex(glm::vec3 position, glm::vec2 uv)
        : position(position),
          normal(0.0f, 0.0f, 0.0f),
          uv(0.0f, 0.0f)
    {
    }

    constexpr Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv)
        : position(position),
          normal(normal),
          uv(uv)
    {
    }
};