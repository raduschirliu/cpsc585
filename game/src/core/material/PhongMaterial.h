#pragma once

#include "core/material/Material.h"

class PhongMaterial final : public BasicMaterial
{
  public:
    PhongMaterial(std::shared_ptr<Texture> diffuse, glm::vec3 specular, float shininess);

    void Apply(glm::mat4 model_matrix) const override;
    ShaderProgram& GetShader() const override;

  private:
    std::shared_ptr<Texture> diffuse_;
    glm::vec3 specular_;
    float shininess_;
};