#pragma once

#include <optional>

#include "core/entity/Camera.h"

class FpsCamera3d : public Camera
{
  public:
    FpsCamera3d(float fov_degrees);

    void Update(float delta_time) override;
    void RenderGui();

    void SetRotation(float pitch_degrees, float yaw_degrees);

    void Reset();
    void SetDefaultOrientation(glm::vec3 position, float pitch_degrees,
                               float yaw_degrees);

  protected:
    void BuildProjectionMatrix() override;
    void BuildViewMatrix() override;

  private:
    float fov_radians_;
    float pitch_degrees_;
    float yaw_degrees_;
    std::optional<glm::vec2> last_mouse_pos_;
    glm::vec3 default_position_;
    float default_pitch_degrees_;
    float default_yaw_degrees_;

    glm::vec3 GetMovementDir();
    void UpdateForwardVector();
};