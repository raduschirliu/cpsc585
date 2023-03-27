#include "engine/render/passes/DepthPass.h"

#include <imgui.h>

#include "engine/core/debug/Assert.h"
#include "engine/core/debug/Log.h"
#include "engine/core/gfx/VertexArray.h"
#include "engine/core/gfx/VertexBuffer.h"
#include "engine/core/gui/PropertyWidgets.h"
#include "engine/render/Camera.h"
#include "engine/render/DebugDrawList.h"
#include "engine/render/MeshRenderer.h"
#include "engine/scene/Entity.h"

using glm::ivec2;
using glm::mat4;
using glm::vec2;
using glm::vec3;
using std::make_unique;
using std::vector;

struct MeshRenderData
{
    const Entity* entity;
    size_t index_count;
    RenderBuffers buffers;
};

static constexpr uint32_t kShadowMapWidth = 4096;
static constexpr uint32_t kShadowMapHeight = 4096;
static vec3 kLightUp(0.0f, 1.0f, 0.0f);
static vec3 kTargetOffset(25.0f, 16.0f, 0.0f);
static vec3 kSourceOffset(60.0f, 25.0f, 0.0f);
static float kNearPlane = 12.5f;
static float kFarPlane = 113.0f;
static vec2 kMapBounds(120.0f, 72.5f);

DepthPass::DepthPass(SceneRenderData& render_data)
    : render_data_(render_data),
      fbo_(),
      depth_map_(),
      shader_("resources/shaders/depth_map.vert",
              "resources/shaders/depth_map.frag"),
      meshes_{},
      debug_draw_bounds_(false),
      target_transform_(nullptr),
      target_pos_(0.0f, 0.0f, 0.0f),
      source_pos_(0.0f, 0.0f, 0.0f)
{
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

    // TODO(radu): this is an ugly hack, fix
    // Check if player vehicle
    if (entity.GetName() == "Player 1")
    {
        target_transform_ = &entity.GetComponent<Transform>();
    }

    // Add to render list
    meshes_.push_back(std::move(data));
}

void DepthPass::UnregisterRenderable(const Entity& entity)
{
    if (entity.GetName() == "Player 1")
    {
        target_transform_ = nullptr;
    }

    const uint32_t target_id = entity.GetId();
    std::erase_if(render_data_.entities, [target_id](const Entity* x)
                  { return x->GetId() == target_id; });
}

void DepthPass::Init()
{
    // Create depth map texture
    glBindTexture(GL_TEXTURE_2D, depth_map_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, kShadowMapWidth,
                 kShadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    const float border_color[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

    // Attatch to FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                           depth_map_, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DepthPass::Render()
{
    glViewport(0, 0, kShadowMapWidth, kShadowMapHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glClear(GL_DEPTH_BUFFER_BIT);

    glCullFace(GL_FRONT);

    RenderMeshes();

    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (target_transform_)
    {
        const vec3 target_offset =
            target_transform_->GetForwardDirection() * kTargetOffset.x +
            target_transform_->GetUpDirection() * kTargetOffset.y +
            target_transform_->GetRightDirection() * kTargetOffset.z;

        target_pos_ = target_transform_->GetPosition() + target_offset;
    }
    else
    {
        target_pos_ = vec3(0.0f);
    }

    source_pos_ = target_pos_ + kSourceOffset;

    // Debug drawing
    if (debug_draw_bounds_)
    {
        const vec3 fwd_dir = glm::normalize(target_pos_ - source_pos_);
        const vec3 right_dir = glm::normalize(glm::cross(fwd_dir, kLightUp));
        const vec3 near_plane_center = source_pos_ + fwd_dir * kNearPlane;
        const vec3 far_plane_center = source_pos_ + fwd_dir * kFarPlane;

        const vec3 x_offset = right_dir * kMapBounds.x / 2.0f;
        const vec3 y_offset = kLightUp * kMapBounds.y / 2.0f;

        const vec3 top_left_offset = -x_offset + y_offset;
        const vec3 bot_left_offset = -x_offset - y_offset;
        const vec3 top_right_offset = x_offset + y_offset;
        const vec3 bot_right_offset = x_offset - y_offset;

        // Connecting
        render_data_.debug_draw_list->AddLine(
            LineVertex(near_plane_center + top_left_offset),
            LineVertex(far_plane_center + top_left_offset));
        render_data_.debug_draw_list->AddLine(
            LineVertex(near_plane_center + top_right_offset),
            LineVertex(far_plane_center + top_right_offset));
        render_data_.debug_draw_list->AddLine(
            LineVertex(near_plane_center + bot_right_offset),
            LineVertex(far_plane_center + bot_right_offset));
        render_data_.debug_draw_list->AddLine(
            LineVertex(near_plane_center + bot_left_offset),
            LineVertex(far_plane_center + bot_left_offset));

        // Near
        render_data_.debug_draw_list->AddLine(
            LineVertex(near_plane_center + top_left_offset),
            LineVertex(near_plane_center + bot_left_offset));
        render_data_.debug_draw_list->AddLine(
            LineVertex(near_plane_center + bot_left_offset),
            LineVertex(near_plane_center + bot_right_offset));
        render_data_.debug_draw_list->AddLine(
            LineVertex(near_plane_center + bot_right_offset),
            LineVertex(near_plane_center + top_right_offset));
        render_data_.debug_draw_list->AddLine(
            LineVertex(near_plane_center + top_right_offset),
            LineVertex(near_plane_center + top_left_offset));

        // Far
        render_data_.debug_draw_list->AddLine(
            LineVertex(far_plane_center + top_left_offset),
            LineVertex(far_plane_center + bot_left_offset));
        render_data_.debug_draw_list->AddLine(
            LineVertex(far_plane_center + bot_left_offset),
            LineVertex(far_plane_center + bot_right_offset));
        render_data_.debug_draw_list->AddLine(
            LineVertex(far_plane_center + bot_right_offset),
            LineVertex(far_plane_center + top_right_offset));
        render_data_.debug_draw_list->AddLine(
            LineVertex(far_plane_center + top_right_offset),
            LineVertex(far_plane_center + top_left_offset));
    }
}

void DepthPass::RenderDebugGui()
{
    if (ImGui::CollapsingHeader("Depth Map"))
    {
        ImGui::Image(depth_map_.ValueRaw(), ImVec2(512, 512));
    }

    ImGui::Checkbox("Draw Shadow Map Bounds", &debug_draw_bounds_);
    gui::EditProperty("Ortho Projection Bounds", kMapBounds);
    gui::EditProperty("Target Offset (Rel)", kTargetOffset);
    gui::EditProperty("Source Offset (Abs)", kSourceOffset);
    ImGui::DragFloat("Near Plane", &kNearPlane, -1000.0f, 1000.0f);
    ImGui::DragFloat("Far Plane", &kFarPlane, -1000.0f, 1000.0f);
}

void DepthPass::ResetState()
{
    meshes_.clear();
}

const TextureHandle& DepthPass::GetDepthMap() const
{
    return depth_map_;
}

mat4 DepthPass::GetLightSpaceTransformation() const
{
    const mat4 light_view = glm::lookAt(source_pos_, target_pos_, kLightUp);

    const mat4 light_proj = glm::ortho(
        -kMapBounds.x / 2.0f, kMapBounds.x / 2.0f, -kMapBounds.y / 2.0f,
        kMapBounds.y / 2.0f, kNearPlane, kFarPlane);

    const mat4 light_space = light_proj * light_view;
    return light_space;
}

void DepthPass::SetDrawDebugBounds(bool state)
{
    debug_draw_bounds_ = state;
}

void DepthPass::RenderMeshes()
{
    const mat4 light_space = GetLightSpaceTransformation();

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
