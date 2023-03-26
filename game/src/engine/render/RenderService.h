#pragma once

#include <memory>
#include <object_ptr.hpp>
#include <vector>

#include "engine/core/gfx/Cubemap.h"
#include "engine/core/gfx/ShaderProgram.h"
#include "engine/core/gfx/VertexArray.h"
#include "engine/core/gfx/VertexBuffer.h"
#include "engine/gui/OnGuiEvent.h"
#include "engine/render/DebugDrawList.h"
#include "engine/render/Material.h"
#include "engine/scene/Entity.h"
#include "engine/service/Service.h"

class Camera;
class Transform;
class MeshRenderer;
class InputService;
class AssetService;

struct BufferMeshLayout
{
    size_t index_offset;
    size_t index_count;
};

struct RenderBuffers
{
    VertexArray vertex_array;
    VertexBuffer vertex_buffer;
    ElementArrayBuffer element_buffer;
};

struct RenderData
{
    const Entity* entity;
    std::vector<BufferMeshLayout> layout;
    VertexArray vertex_array;
    VertexBuffer vertex_buffer;
    ElementArrayBuffer element_buffer;
};

struct RenderPassData
{
    Transform* camera_transform;
    glm::vec3 camera_pos;
    glm::mat4 view_matrix;
    glm::mat4 proj_matrix;
    glm::mat4 view_proj_matrix;
};

class RenderService final : public Service, public IEventSubscriber<OnGuiEvent>
{
  public:
    RenderService();

    void RegisterRenderable(const Entity& entity, const MeshRenderer& renderer);
    void UnregisterRenderable(const Entity& entity);
    void RegisterCamera(Camera& camera);
    void UnregisterCamera(Camera& camera);
    void RegisterLight(Entity& entity);
    void UnregisterLight(Entity& entity);

    // From Service
    void OnInit() override;
    void OnStart(ServiceProvider& service_provider) override;
    void OnSceneLoaded(Scene& scene) override;
    void OnUpdate() override;
    void OnCleanup() override;
    std::string_view GetName() const override;

    // From IEventSubscriber<OnGuiEvent>
    void OnGui() override;

    DebugDrawList& GetDebugDrawList();

  private:
    jss::object_ptr<InputService> input_service_;
    jss::object_ptr<AssetService> asset_service_;

    std::vector<std::unique_ptr<RenderData>> render_list_;
    std::vector<jss::object_ptr<Camera>> cameras_;
    std::vector<jss::object_ptr<Entity>> lights_;
    std::vector<std::unique_ptr<Material>> materials_;
    ShaderProgram shader_, debug_shader_, skybox_shader_;
    RenderPassData render_pass_data_;
    RenderBuffers skybox_buffers_;
    const Cubemap* skybox_texture_;
    DebugDrawList debug_draw_list_;
    bool wireframe_;
    bool show_debug_menu_;
    size_t num_draw_calls_;

    void RenderPrepare();
    void PrepareCameraView(Camera& camera);
    void RenderCameraView(Camera& camera);
    void RenderDebugDrawList();
    void RenderSkybox();

    void RegisterMaterial(std::unique_ptr<Material> material);
};
