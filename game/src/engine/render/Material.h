#pragma once

#include <glm/glm.hpp>
#include <optional>
#include <string>

class Texture;

struct MaterialProperties
{
    const Texture* albedo_texture;
    glm::vec3 albedo_color;
    glm::vec3 specular;
    float shininess;
};
