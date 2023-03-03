#pragma once

#include "engine/core/gfx/ShaderProgram.h"

struct MaterialProperties
{
    glm::vec3 albedo_color;
    glm::vec3 specular;
    float shininess;
};

class Material
{
  public:
    virtual void Draw();
};
