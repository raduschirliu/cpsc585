#pragma once

#include "core/material/Material.h"

class TextureMaterial final : public BasicMaterial
{
  public:
    TextureMaterial(std::shared_ptr<Texture> diffuse);

    void Apply(glm::mat4 model_matrix) const override;
    ShaderProgram& GetShader() const override;

  private:
    std::shared_ptr<Texture> diffuse_;
};