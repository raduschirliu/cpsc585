#include "engine/render/passes/DepthPass.h"

#include <imgui.h>

#include <limits>

#include "engine/core/debug/Assert.h"
#include "engine/core/debug/Log.h"
#include "engine/core/gfx/VertexArray.h"
#include "engine/core/gfx/VertexBuffer.h"
#include "engine/core/gui/PropertyWidgets.h"
#include "engine/core/math/Rect2d.h"
#include "engine/render/Camera.h"
#include "engine/render/DebugDrawList.h"
#include "engine/render/MeshRenderer.h"
#include "engine/render/passes/depth/ShadowMap.h"
#include "engine/scene/Entity.h"

using glm::ivec2;
using glm::mat4;
using glm::uvec2;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using std::make_unique;
using std::unique_ptr;
using std::vector;

struct MeshRenderData
{
    const Entity* entity;
    size_t index_count;
    RenderBuffers buffers;
};

static ShadowMap::LightParams kLightParams = {
    .up_dir = vec3(0.0f, 1.0f, 0.0f),
    .pos = vec3(45.0f, 20.0f, 0.0f),
};

DepthPass::DepthPass(SceneRenderData& render_data)
    : render_data_(render_data),
      shadow_maps_{},
      shader_("resources/shaders/depth_map.vert",
              "resources/shaders/depth_map.frag"),
      meshes_{},
      debug_draw_shadow_bounds_(false),
      debug_draw_camera_bounds_(false),
      current_camera_(nullptr)
{
    shadow_maps_.emplace_back(make_unique<ShadowMap>(ShadowMapParams{
        .texture_size = uvec2(4096, 4096),
        .camera_near_plane = 0.5f,
        .camera_far_plane = 80.0f,
        .camera_midpoint_t = 0.2f,
        .bounds_min = vec3(-100.0f, -64.0f, -20.0f),
        .bounds_max = vec3(100.0f, 64.0f, 100.0f),
        .bounds_mult = vec3(1.0f, 1.0f, 1.0f),
        .cull_face = true,
    }));
    shadow_maps_.emplace_back(make_unique<ShadowMap>(ShadowMapParams{
        .texture_size = uvec2(2048, 2048),
        .camera_near_plane = 50.0f,
        .camera_far_plane = 300.0f,
        .camera_midpoint_t = 0.25f,
        .bounds_min = vec3(-200.0f, -200.0f, -50.0f),
        .bounds_max = vec3(200.0f, 200.0f, 100.0f),
        .bounds_mult = vec3(1.0f, 1.0f, 1.0f),
        .cull_face = true,
    }));
}

DepthPass::~DepthPass() = default;

void DepthPass::RegisterRenderable(const Entity& entity,
                                   const MeshRenderer& renderer)
{
    auto data = make_unique<MeshRenderData>();
    data->entity = &entity;

    // Configure vertex array/buffer and upload data
    data->buffers.vertex_array.Bind();
    data->buffers.vertex_buffer.Bind();
    data->buffers.element_buffer.Bind();

    data->buffers.vertex_buffer.ConfigureAttribute(
        0, 3, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, position));

    constexpr size_t index_size = sizeof(uint32_t);
    vector<Vertex> vertices;
    vector<uint32_t> indices;

    for (const auto& mesh : renderer.GetMeshes())
    {
        const uint32_t vertex_offset = static_cast<uint32_t>(vertices.size());
        const uint32_t index_offset = static_cast<uint32_t>(indices.size());
        const size_t index_count = mesh.mesh->indices.size();

        vertices.insert(vertices.end(), mesh.mesh->vertices.begin(),
                        mesh.mesh->vertices.end());
        indices.insert(indices.end(), mesh.mesh->indices.begin(),
                       mesh.mesh->indices.end());

        for (size_t i = 0; i < index_count; i++)
        {
            indices[index_offset + i] += vertex_offset;
        }
    }

    data->buffers.vertex_buffer.Upload(vertices, GL_STATIC_DRAW);
    data->buffers.element_buffer.Upload(indices, GL_STATIC_DRAW);
    data->index_count = indices.size();

    VertexArray::Unbind();

    // Add to render list
    meshes_.push_back(std::move(data));
}

void DepthPass::UnregisterRenderable(const Entity& entity)
{
    const uint32_t target_id = entity.GetId();
    std::erase_if(render_data_.entities, [target_id](const Entity* x)
                  { return x->GetId() == target_id; });
}

void DepthPass::Init()
{
    for (auto& shadow_map : shadow_maps_)
    {
        shadow_map->Init();
    }
}

void DepthPass::Render()
{
    if (ShouldRun())
    {
        for (Camera* camera : render_data_.cameras)
        {
            // Only draw shadows for "normal" camera types
            if (camera->GetType() != CameraType::kNormal)
            {
                continue;
            }

            current_camera_ = camera;
            RenderShadowMaps();
        }
    }

    // Reset framebuffer & culling settings
    glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DepthPass::RenderDebugGui()
{
    for (size_t i = 0; i < shadow_maps_.size(); i++)
    {
        auto& map = shadow_maps_[i];

        if (ImGui::TreeNode(&map, "Shadow Map %u", i))
        {
            ShadowMapParams& params = map->GetParams();

            ImGui::DragFloat("Camera Near Plane", &params.camera_near_plane,
                             1.0f, -1000.0f, 1000.0f);
            ImGui::DragFloat("Camera Far Plane", &params.camera_far_plane, 1.0f,
                             -1000.0f, 1000.0f);
            ImGui::DragFloat("Camera Midpoint t", &params.camera_midpoint_t,
                             0.05f, 0.0f, 1.0f);
            gui::EditProperty("Bounds Min", params.bounds_min);
            gui::EditProperty("Bounds Max", params.bounds_max);
            gui::EditProperty("Bounds Mult", params.bounds_mult);
            ImGui::Text("Texture Size: %u %u", params.texture_size.x,
                        params.texture_size.y);
            ImGui::Image(map->GetTexture().ValueRaw(), ImVec2(512, 512));
            ImGui::TreePop();
        }
    }

    ImGui::Checkbox("Draw Shadow Map Bounds", &debug_draw_shadow_bounds_);
    ImGui::Checkbox("Draw Camera Frustum Bounds", &debug_draw_camera_bounds_);
    gui::EditProperty("Light Pos", kLightParams.pos);
}

void DepthPass::ResetState()
{
    meshes_.clear();
}

const vector<unique_ptr<ShadowMap>>& DepthPass::GetShadowMaps() const
{
    return shadow_maps_;
}

void DepthPass::RenderShadowMaps()
{
    ASSERT(current_camera_);

    const vec3& camera_pos =
        current_camera_->GetEntity().GetComponent<Transform>().GetPosition();

    ShadowMap::CameraParams camera_params = {
        .pos = camera_pos,
        .view_matrix = current_camera_->GetViewMatrix(),
        .fov_radians = glm::radians(current_camera_->GetFovDegrees()),
        .aspect_ratio = current_camera_->GetAspectRatio()};

    for (auto& shadow_map : shadow_maps_)
    {
        shadow_map->Prepare();
        shadow_map->UpdateBounds(kLightParams, camera_params);
        RenderMeshes(*shadow_map);

        if (debug_draw_shadow_bounds_)
        {
            RenderDebugShadowBounds(*shadow_map);
        }

        if (debug_draw_camera_bounds_)
        {
            RenderDebugCameraBounds(*shadow_map);
        }
    }
}

void DepthPass::RenderMeshes(ShadowMap& shadow_map)
{
    ASSERT(current_camera_);

    const mat4& light_space = shadow_map.GetTransformation();

    shader_.Use();
    shader_.SetUniform("uLightSpaceMatrix", light_space);

    // Render each object
    for (const auto& obj : meshes_)
    {
        const MeshRenderer& renderer =
            obj->entity->GetComponent<MeshRenderer>();
        const Transform& transform = obj->entity->GetComponent<Transform>();

        const mat4& model_matrix = transform.GetModelMatrix();

        // Vert shader vars
        shader_.SetUniform("uModelMatrix", model_matrix);

        obj->buffers.vertex_array.Bind();

        // Draw all meshes that are part of this object
        const GLsizei index_count = static_cast<GLsizei>(obj->index_count);
        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
    }
}

void DepthPass::RenderDebugShadowBounds(ShadowMap& shadow_map)
{
    constexpr Color4u kDebugColor(255, 255, 0, 255);

    render_data_.debug_draw_list->AddCuboid(shadow_map.GetShadowBounds(),
                                            kDebugColor);
}

void DepthPass::RenderDebugCameraBounds(ShadowMap& shadow_map)
{
    constexpr Color4u kDebugColor(0, 255, 255, 255);

    render_data_.debug_draw_list->AddCuboid(shadow_map.GetCameraBounds(),
                                            kDebugColor);

    const vec3& camera_pos =
        current_camera_->GetEntity().GetComponent<Transform>().GetPosition();

    render_data_.debug_draw_list->AddLine(
        DebugVertex(camera_pos, kDebugColor),
        DebugVertex(shadow_map.GetTargetPos(), kDebugColor));
}

bool DepthPass::ShouldRun()
{
    return render_data_.cameras.size() > 0;
}
