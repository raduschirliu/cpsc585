#pragma once

#include "core/entity/GameObject.h"

class NormalMarkerObject final : public GameObject
{
  public:
    NormalMarkerObject(GameObject& target_obj);

  private:
    std::shared_ptr<::Geometry> target_geometry_;
    glm::vec3 color_;

    void BuildGeometry();
};