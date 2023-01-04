#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "core/gfx/ShaderProgram.h"
#include "core/gfx/Texture.h"

class IMaterial
{
  public:
    virtual void Update(float delta_time) = 0;
    virtual void Apply(glm::mat4 model_matrix) const = 0;
    virtual ShaderProgram& GetShader() const = 0;
};

class BasicMaterial : public IMaterial
{
  public:
    void Update(float delta_time) override;
    void Apply(glm::mat4 model_matrix) const override;
    ShaderProgram& GetShader() const override;
};
