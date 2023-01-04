#pragma once

#include <glm/glm.hpp>

struct Vertex
{
    // Off white
    static constexpr glm::vec3 kDefaultColor = glm::vec3(0.9f, 0.9f, 0.9f);

    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    glm::vec2 uv;

    explicit constexpr Vertex(glm::vec3 position)
        : position(position),
          normal(0.0f, 0.0f, 0.0f),
          color(kDefaultColor),
          uv(0.0f, 0.0f)
    {
    }

    constexpr Vertex(glm::vec3 position, glm::vec2 uv)
        : position(position),
          normal(0.0f, 0.0f, 0.0f),
          color(kDefaultColor),
          uv(0.0f, 0.0f)
    {
    }

    constexpr Vertex(glm::vec3 position, glm::vec3 color)
        : position(position),
          normal(0.0f, 0.0f, 0.0f),
          color(color),
          uv(0.0f, 0.0f)
    {
    }

    constexpr Vertex(glm::vec3 position, glm::vec3 normal, glm::vec3 color,
                     glm::vec2 uv)
        : position(position),
          normal(normal),
          color(color),
          uv(uv)
    {
    }
};