#pragma once

#include "core/entity/GameObject.h"

class OrbitMarkerObject final : public GameObject
{
  public:
    OrbitMarkerObject(const glm::vec3& axis, float length,
                      const glm::vec3& color);

  private:
    float length_;
    glm::vec3 axis_;
    glm::vec3 color_;

    void BuildGeometry();
};