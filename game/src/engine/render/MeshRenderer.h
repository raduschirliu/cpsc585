#pragma once

#include <object_ptr.hpp>
#include <optional>
#include <string>
#include <vector>

#include "engine/asset/AssetService.h"
#include "engine/render/Material.h"
#include "engine/scene/Component.h"
#include "engine/scene/Transform.h"

class RenderService;

struct RenderableMesh
{
    std::string name;
    MaterialProperties properties;
};

class MeshRenderer final : public Component
{
  public:
    MeshRenderer() = default;

    void SetMesh(const std::string& name);
    void SetMaterialProperties(const MaterialProperties& material_properties);
    void SetMaterial(const std::string& name);

    void AddMesh(const RenderableMesh& mesh);
    const std::vector<RenderableMesh>& GetMeshes() const;

    const Mesh& GetMesh() const;
    const MaterialProperties& GetMaterialProperties() const;
    const Material& GetMaterial() const;

    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    void OnDebugGui() override;
    void OnDestroy() override;
    std::string_view GetName() const override;

  private:
    jss::object_ptr<RenderService> render_service_;
    jss::object_ptr<AssetService> asset_service_;
    jss::object_ptr<Transform> transform_;

    std::vector<RenderableMesh> meshes_;
    jss::object_ptr<Material> material_;
};
