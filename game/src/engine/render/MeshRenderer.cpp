#include "engine/render/MeshRenderer.h"

#include <assimp/postprocess.h>  // Post processing flags
#include <assimp/scene.h>        // Output data structure
#include <imgui.h>

#include <assimp/Importer.hpp>  // C++ importer interface
#include <glm/gtc/matrix_transform.hpp>

#include "engine/core/debug/Log.h"
#include "engine/render/RenderService.h"
#include "engine/scene/Entity.h"

using glm::mat4;
using std::string;

void MeshRenderer::SetMesh(const string& name)
{
    if (mesh_name_)
    {
        render_service_->UnregisterRenderable(GetEntity());
    }

    mesh_name_ = name;
    render_service_->RegisterRenderable(GetEntity());
}

const Mesh& MeshRenderer::GetMesh() const
{
    ASSERT_MSG(mesh_name_.has_value(), "No mesh assigned");
    return asset_service_->GetMesh(mesh_name_.value());
}

void MeshRenderer::OnInit(const ServiceProvider& service_provider)
{
    // Get dependencies
    render_service_ = &service_provider.GetService<RenderService>();
    asset_service_ = &service_provider.GetService<AssetService>();

    transform_ = &GetEntity().GetComponent<Transform>();
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
}

std::string_view MeshRenderer::GetName() const
{
    return "MeshRenderer";
}
