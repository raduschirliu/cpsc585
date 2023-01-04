#pragma once

#include "core/ShaderManager.h"
#include "core/entity/Transform.h"

class Camera
{
  public:
    Camera();

    virtual void RenderGui();
    virtual void Update(float delta_time);
    virtual void Render();

    void SetViewportSize(glm::ivec2 size);
    void UpdateShaderUniforms();

    ::Transform& Transform()
    {
        return transform_;
    }

  protected:
    ::Transform transform_;
    glm::vec2 viewport_size_;
    glm::mat4 view_matrix_, projection_matrix_;

    virtual void BuildProjectionMatrix() = 0;
    virtual void BuildViewMatrix() = 0;
};