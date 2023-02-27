#pragma once

#include "engine/core/gfx/ShaderProgram.h"

struct MaterialProperties
{
    glm::vec3 albedo_color;
    glm::vec3 diffuse;
    glm::vec3 ambient;
    glm::vec3 specular;
    float shininess;
};

class Material
{
  public:
    void DrawMesh();  // TODO(radu): what should this look like?

  private:
    ShaderProgram shader_;
};
