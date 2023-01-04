#pragma once

#include "core/entity/GameObject.h"
#include "objects/AxisMarkerObject.h"

class PlanetObject final : public GameObject
{
  public:
    PlanetObject(float radius);

    // From GameObject
    void Update(float delta_time) override;

    void SetAxisMarkerVisible(bool visible);
    void SetWireframe(bool enabled);
    void SetAxialRotation(float axial_tilt_rad, float rotation_speed_rad);
    void SetOrbitalRevolution(std::shared_ptr<PlanetObject> orbit_target,
                              float orbital_radius,
                              float orbital_inclination_rad,
                              float orbital_speed_rad);

    void Reset();

  protected:
    // From GameObject
    glm::mat4 BuildModelMatrix() override;

  private:
    ::Transform rotation_transform_;
    float radius_;
    glm::vec4 rotation_axis_;
    float rotation_speed_rad_;
    float orbital_speed_rad_;
    float orbital_radius_;
    float orbital_inclination_height_max_;
    float orbit_time_;
    std::shared_ptr<PlanetObject> orbit_target_;
    std::shared_ptr<AxisMarkerObject> axis_marker_obj_;

    void BuildGeometry();
    uint32_t GetIndex(size_t num_cols, uint32_t row, uint32_t col) const;
};