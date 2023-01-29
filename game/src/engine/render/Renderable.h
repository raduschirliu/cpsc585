#pragma once

#include <glm/glm.hpp>

#include "engine/render/Mesh.h"
#include "engine/scene/Component.h"

class RenderableComponent : public Component
{
  public:
    // virtual const Mesh& GetMesh() const = 0;
    // virtual const glm::mat4& GetModelMatrix() const = 0;
};