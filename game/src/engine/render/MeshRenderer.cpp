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
using std::vector;

static const MaterialProperties kDefaultMaterialProperties = {
    nullptr, vec3(0.8f, 0.8f, 0.8f), vec3(0.8f, 0.8f, 0.8f), 64.0f};

void MeshRenderer::OnInit(const ServiceProvider& service_provider)
{
    // Services
    render_service_ = &service_provider.GetService<RenderService>();
    asset_service_ = &service_provider.GetService<AssetService>();

    // Components
    transform_ = &GetEntity().GetComponent<Transform>();

    // Set defaults
    SetMaterial("default");
}

void MeshRenderer::OnDebugGui()
{
    for (auto& mesh : meshes_)
    {
        ImGui::PushID(&mesh);

        if (ImGui::TreeNode(&mesh, "%s", mesh.mesh->name.c_str()))
        {
            if (mesh.material_properties.albedo_texture)
            {
                ImGui::Image(
                    mesh.material_properties.albedo_texture->GetGuiHandle(),
                    ImVec2(150.0f, 150.0f));
            }
            else
            {
                ImGui::Text("Texture: None");
            }

            gui::EditColorProperty("Albedo",
                                   mesh.material_properties.albedo_color);
            gui::EditColorProperty("Specular",
                                   mesh.material_properties.specular);
            ImGui::DragFloat("Shininess", &mesh.material_properties.shininess,
                             0.5f, 1.0f, 512.0f);
            ImGui::TreePop();
        }

        ImGui::PopID();
    }
}

void MeshRenderer::OnDestroy()
{
    render_service_->UnregisterRenderable(GetEntity());
}

std::string_view MeshRenderer::GetName() const
{
    return "MeshRenderer";
}

void MeshRenderer::SetMesh(const RenderableMesh& mesh)
{
    if (meshes_.size() > 0)
    {
        render_service_->UnregisterRenderable(GetEntity());
        meshes_.clear();
    }

    ASSERT_MSG(mesh.mesh, "Must have valid mesh data");

    meshes_ = {mesh};
    render_service_->RegisterRenderable(GetEntity(), *this);
}

void MeshRenderer::SetMeshes(const vector<RenderableMesh>& meshes)
{
    if (meshes_.size() > 0)
    {
        render_service_->UnregisterRenderable(GetEntity());
        meshes_.clear();
    }

    for (const auto& mesh : meshes)
    {
        ASSERT_MSG(mesh.mesh, "Must have valid mesh data");
    }

    meshes_ = meshes;
    render_service_->RegisterRenderable(GetEntity(), *this);
}

void MeshRenderer::SetMaterial(const string& name)
{
    // TODO(radu): get material by name from render service and set
}

const Material& MeshRenderer::GetMaterial() const
{
    ASSERT_MSG(material_, "Must have a valid material set");
    return *material_;
}

const vector<RenderableMesh>& MeshRenderer::GetMeshes() const
{
    return meshes_;
}
