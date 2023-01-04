#pragma once

#include <optional>

#include "core/entity/Camera.h"

class OrbitCamera3d : public Camera
{
  public:
    OrbitCamera3d(float fov_degrees, float orbit_radius);

    void Update(float delta_time) override;
    void RenderGui();

    void Reset();

  protected:
    void BuildProjectionMatrix() override;
    void BuildViewMatrix() override;

  private:
    float fov_radians_;
    float theta_rad_;
    float phi_rad_;
    float orbit_radius_;
    float default_orbit_radius_;
    std::optional<glm::vec2> last_mouse_pos_;

    void Rotate(float phi_delta, float theta_delta);
    float GetZoomChangeDir();
};