#pragma once

#include "core/entity/GameObject.h"

class AxisMarkerObject final : public GameObject
{
  public:
    AxisMarkerObject(const glm::vec3& axis, bool centered, float length,
                     const glm::vec3& color);

  private:
    float length_;
    bool centered_;
    glm::vec3 axis_;
    glm::vec3 color_;

    void BuildGeometry();
};