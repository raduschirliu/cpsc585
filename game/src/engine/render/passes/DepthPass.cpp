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
#include "engine/scene/Entity.h"

using glm::ivec2;
using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using std::make_unique;
using std::vector;

struct MeshRenderData
{
    const Entity* entity;
    size_t index_count;
    RenderBuffers buffers;
};

static constexpr float kFloatMax = std::numeric_limits<float>::max();
static constexpr float kFloatMin = std::numeric_limits<float>::lowest();
static constexpr uint32_t kShadowMapWidth = 2048;
static constexpr uint32_t kShadowMapHeight = 2048;
static vec3 kLightUp(0.0f, 1.0f, 0.0f);
static vec3 kLightPos(45.0f, 20.0f, 0.0f);
static float kNearPlane = 0.5f;
static float kFarPlane = 110.0f;
static vec3 kBoundsMult(1.0f, 1.0f, 0.0f);

DepthPass::DepthPass(SceneRenderData& render_data)
    : render_data_(render_data),
      fbo_(),
      depth_map_(),
      shader_("resources/shaders/depth_map.vert",
              "resources/shaders/depth_map.frag"),
      meshes_{},
      debug_draw_bounds_(false),
      debug_draw_frustum_segments_(false),
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

    if (target_transform_)
    {
        RenderPrepare();
        RenderMeshes();
        RenderDebugBounds();
    }

    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DepthPass::RenderDebugGui()
{
    if (ImGui::CollapsingHeader("Depth Map"))
    {
        ImGui::Image(depth_map_.ValueRaw(), ImVec2(512, 512));
    }

    ImGui::Checkbox("Draw Shadow Map Bounds", &debug_draw_bounds_);
    ImGui::Checkbox("Draw Camera Frustum Segments",
                    &debug_draw_frustum_segments_);
    gui::EditProperty("Light Pos", kLightPos);
    gui::EditProperty("Bounds Multiplier", kBoundsMult);
    ImGui::DragFloat("Near Plane", &kNearPlane, 1.0f, -1000.0f, 1000.0f);
    ImGui::DragFloat("Far Plane", &kFarPlane, 1.0f, -1000.0f, 1000.0f);
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
    return light_proj_ * light_view_;
}

void DepthPass::RenderPrepare()
{
    // Determine target position
    Camera* camera = render_data_.cameras[0];
    const mat4 camera_proj_segment = glm::perspective(
        glm::radians(90.0f), 1280.0f / 720.0f, kNearPlane, kFarPlane);
    Cuboid frustum;
    frustum.BoundsFromNdcs(camera_proj_segment * camera->GetViewMatrix());

    if (debug_draw_frustum_segments_)
    {
        constexpr Color4u kDebugColor(0, 255, 255, 255);
        const vec3& camera_pos =
            camera->GetEntity().GetComponent<Transform>().GetPosition();

        render_data_.debug_draw_list->AddCuboid(frustum, kDebugColor);
        render_data_.debug_draw_list->AddLine(
            DebugVertex(camera_pos, kDebugColor),
            DebugVertex(target_pos_, kDebugColor));
    }

    target_pos_ = frustum.GetCentroidMidpoint(0.5f);
    source_pos_ = target_pos_ + kLightPos;

    // Build proj and view matrices
    light_view_ = glm::lookAt(source_pos_, target_pos_, kLightUp);

    // TODO(radu): Do do cascaded shadow maps, need to calc various different
    // proj matrices for the camera (with different values for near/far), and
    // perform these calcs for each subsection to get a different ortho matrix +
    // CSM for each one
    vec3 min(kFloatMax, kFloatMax, kFloatMax);
    vec3 max(kFloatMin, kFloatMin, kFloatMin);
    const vec3* vertex = frustum.GetVertexList();

    for (size_t i = 0; i < frustum.GetVertexCount(); i++)
    {
        const vec4 temp = light_view_ * vec4(*vertex, 1.0f);

        min = glm::min(min, vec3(temp));
        max = glm::max(max, vec3(temp));

        vertex++;
    }

    if (min.x < 0.0f)
    {
        min.x *= kBoundsMult.x;
    }
    else
    {
        min.x /= kBoundsMult.x;
    }
    if (max.x < 0.0f)
    {
        max.x /= kBoundsMult.x;
    }
    else
    {
        max.x *= kBoundsMult.x;
    }

    if (min.y < 0.0f)
    {
        min.y *= kBoundsMult.y;
    }
    else
    {
        min.y /= kBoundsMult.y;
    }
    if (max.y < 0.0f)
    {
        max.y /= kBoundsMult.y;
    }
    else
    {
        max.y *= kBoundsMult.y;
    }

    if (min.z < 0.0f)
    {
        min.z *= kBoundsMult.z;
    }
    else
    {
        min.z /= kBoundsMult.z;
    }
    if (max.z < 0.0f)
    {
        max.z /= kBoundsMult.z;
    }
    else
    {
        max.z *= kBoundsMult.z;
    }

    static constexpr vec3 kSmallest(-200.0f, -200.0f, -200.0f);
    static constexpr vec3 kLargest(200.0f, 200.0f, 200.0f);

    max = glm::clamp(max, kSmallest, kLargest);
    min = glm::clamp(min, kSmallest, kLargest);

    const float near_plane = glm::min(-25.0f, min.z);
    const float far_plane = glm::max(125.0f, max.z);

    light_proj_ = glm::ortho(min.x, max.x, min.y, max.y, near_plane, far_plane);
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

void DepthPass::RenderDebugBounds()
{
    if (!debug_draw_bounds_)
    {
        return;
    }

    Cuboid bounds;
    bounds.BoundsFromNdcs(light_proj_ * light_view_);
    render_data_.debug_draw_list->AddCuboid(bounds, Color4u(255, 255, 0, 255));
}