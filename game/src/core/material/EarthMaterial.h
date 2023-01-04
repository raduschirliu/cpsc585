#pragma once

#include "core/material/Material.h"

class EarthMaterial final : public BasicMaterial
{
  public:
    EarthMaterial(std::shared_ptr<Texture> diffuse,
                  std::shared_ptr<Texture> diffuse_night,
                  std::shared_ptr<Texture> diffuse_clouds_,
                  std::shared_ptr<Texture> specular, float shininess);

    // From IMaterial
    void Update(float delta_time) override;
    void Apply(glm::mat4 model_matrix) const override;
    ShaderProgram& GetShader() const override;

  private:
    std::shared_ptr<Texture> diffuse_;
    std::shared_ptr<Texture> diffuse_night_;
    std::shared_ptr<Texture> diffuse_clouds_;
    std::shared_ptr<Texture> specular_;
    float shininess_;
    float time_;
};