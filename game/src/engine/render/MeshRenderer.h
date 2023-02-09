#pragma once

#include <object_ptr.hpp>
#include <optional>
#include <string>

#include "engine/asset/AssetService.h"
#include "engine/scene/Component.h"
#include "engine/scene/Transform.h"

class RenderService;

class MeshRenderer final : public Component
{
  public:
    MeshRenderer() = default;

    void SetMesh(const std::string& name);

    // From RenderableComponent
    const Mesh& GetMesh() const;

    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    void OnDebugGui();
    std::string_view GetName() const override;

  private:
    jss::object_ptr<RenderService> render_service_;
    jss::object_ptr<AssetService> asset_service_;
    jss::object_ptr<Transform> transform_;

    std::optional<std::string> mesh_name_;
};
