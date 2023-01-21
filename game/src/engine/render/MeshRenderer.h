#pragma once

#include <object_ptr.hpp>

#include "engine/render/Renderable.h"
#include "engine/scene/Component.h"

class RenderService;

class MeshRenderer final : public RenderableComponent
{
  public:
    MeshRenderer() = default;

    // From RenderableComponent
    const Mesh& GetMesh() const override;
    const glm::mat4 GetModelMatrix() const override;

    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    std::string_view GetName() const override;

  private:
    Mesh mesh_;
    jss::object_ptr<RenderService> render_service_;
};
