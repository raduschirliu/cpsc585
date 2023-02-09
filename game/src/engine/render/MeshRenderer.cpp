#include "engine/render/MeshRenderer.h"

#include <assimp/postprocess.h>  // Post processing flags
#include <assimp/scene.h>        // Output data structure
#include <imgui.h>

#include <assimp/Importer.hpp>  // C++ importer interface
#include <glm/gtc/matrix_transform.hpp>

#include "engine/core/debug/Log.h"
#include "engine/core/gui/PropertyWidgets.h"
#include "engine/render/RenderService.h"
#include "engine/scene/Entity.h"

using glm::mat4;
using glm::vec3;
using std::string;

void MeshRenderer::OnInit(const ServiceProvider& service_provider)
{
    // Get dependencies
    render_service_ = &service_provider.GetService<RenderService>();
    asset_service_ = &service_provider.GetService<AssetService>();

    transform_ = &GetEntity().GetComponent<Transform>();

    // Set defaults
    SetMaterial("default");
    SetMaterialProperties({.albedo_color = vec3(1.0f, 1.0f, 1.0f),
                           .specular = vec3(0.5f, 0.5f, 0.5f),
                           .shininess = 64.0f});
}

void MeshRenderer::OnDebugGui()
{
    if (mesh_name_)
    {
        ImGui::Text("Mesh: %s", mesh_name_.value().c_str());
    }
    else
    {
        ImGui::Text("Mesh: %s", "<NONE>");
    }

    gui::Vec3Field("Albedo", material_properties_.albedo_color);
    gui::Vec3Field("Specular", material_properties_.specular);
}

std::string_view MeshRenderer::GetName() const
{
    return "MeshRenderer";
}

void MeshRenderer::SetMesh(const string& name)
{
    if (mesh_name_)
    {
        render_service_->UnregisterRenderable(GetEntity());
    }

    mesh_name_ = name;
    render_service_->RegisterRenderable(GetEntity());
}

void MeshRenderer::SetMaterialProperties(
    const MaterialProperties& material_properties)
{
    material_properties_ = material_properties;
}

void MeshRenderer::SetMaterial(const string& name)
{
    // TODO(radu): get material by name from render service and set
}

const Mesh& MeshRenderer::GetMesh() const
{
    ASSERT_MSG(mesh_name_.has_value(), "No mesh assigned");
    return asset_service_->GetMesh(mesh_name_.value());
}

const Material& MeshRenderer::GetMaterial() const
{
    ASSERT_MSG(material_, "Must have a valid material set");
    return *material_;
}

const MaterialProperties& MeshRenderer::GetMaterialProperties() const
{
    return material_properties_;
}
