#include "engine/render/RenderService.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>

#include "engine/asset/AssetService.h"
#include "engine/core/debug/Log.h"
#include "engine/input/InputService.h"
#include "engine/render/Camera.h"
#include "engine/render/Material.h"
#include "engine/render/MeshRenderer.h"
#include "engine/render/PointLight.h"
#include "engine/service/ServiceProvider.h"

using glm::ivec2;
using glm::mat4;
using glm::vec3;
using std::make_unique;
using std::unique_ptr;
using std::vector;

RenderService::RenderService()
    : input_service_(nullptr),
      asset_service_(nullptr),
      render_data_(make_unique<SceneRenderData>()),
      depth_pass_(*render_data_),
      geometry_pass_(*render_data_),
      debug_draw_list_(),
      show_debug_menu_(false),
      debug_draw_camera_frustums_(false)
{
}

void RenderService::RegisterRenderable(const Entity& entity,
                                       const MeshRenderer& renderer)
{
    const uint32_t new_id = entity.GetId();
    auto iter = std::find_if(
        render_data_->entities.begin(), render_data_->entities.end(),
        [new_id](const Entity* x) { return x->GetId() == new_id; });
    ASSERT_MSG(iter == render_data_->entities.end(),
               "Cannot register the same entity twice");

    render_data_->entities.push_back(&entity);

    depth_pass_.RegisterRenderable(entity, renderer);
    geometry_pass_.RegisterRenderable(entity, renderer);
}

void RenderService::UnregisterRenderable(const Entity& entity)
{
    const uint32_t target_id = entity.GetId();
    size_t count =
        std::erase_if(render_data_->entities, [target_id](const Entity* x)
                      { return x->GetId() == target_id; });

    if (count > 0)
    {
        depth_pass_.UnregisterRenderable(entity);
        geometry_pass_.UnregisterRenderable(entity);
    }

    debug::LogWarn(
        "Attempted to unregister a renderable entity that was never "
        "registered");
}

void RenderService::RegisterCamera(Camera& camera)
{
    const float aspect_ratio = static_cast<float>(render_data_->screen_size.x) /
                               static_cast<float>(render_data_->screen_size.y);
    camera.SetAspectRatio(aspect_ratio);

    render_data_->cameras.push_back(&camera);
}

void RenderService::UnregisterCamera(Camera& camera)
{
    const uint32_t target_id = camera.GetEntity().GetId();
    std::erase_if(render_data_->cameras, [target_id](Camera* x)
                  { return x->GetEntity().GetId() == target_id; });
}

void RenderService::RegisterLight(Entity& entity)
{
    const uint32_t new_id = entity.GetId();
    auto iter = std::find_if(
        render_data_->point_lights.begin(), render_data_->point_lights.end(),
        [new_id](PointLight* x) { return x->GetEntity().GetId() == new_id; });
    ASSERT_MSG(iter == render_data_->point_lights.end(),
               "Cannot register the same light twice");

    // TODO(radu): Support directional lights?
    render_data_->point_lights.push_back(&entity.GetComponent<PointLight>());
}

void RenderService::UnregisterLight(Entity& entity)
{
    // TODO(radu): Support directional lights?

    const uint32_t target_id = entity.GetId();
    std::erase_if(render_data_->point_lights, [target_id](PointLight* x)
                  { return x->GetEntity().GetId() == target_id; });
}

void RenderService::OnInit()
{
}

void RenderService::OnStart(ServiceProvider& service_provider)
{
    // Service dependencies
    input_service_ = &service_provider.GetService<InputService>();
    asset_service_ = &service_provider.GetService<AssetService>();

    // Events
    GetEventBus().Subscribe<OnGuiEvent>(this);

    // Render passes
    render_data_->asset_service = asset_service_.get();
    render_data_->debug_draw_list = &debug_draw_list_;

    depth_pass_.Init();
    geometry_pass_.Init();
}

void RenderService::OnSceneLoaded(Scene& scene)
{
    depth_pass_.ResetState();
    geometry_pass_.ResetState();

    render_data_->cameras.clear();
    render_data_->entities.clear();
    render_data_->point_lights.clear();
}

void RenderService::OnWindowSizeChanged(int width, int height)
{
    debug::LogInfo("Window size changed: {}x{}", width, height);
    render_data_->screen_size = ivec2(width, height);

    const float aspect_ratio =
        static_cast<float>(width) / static_cast<float>(height);

    for (auto& camera : render_data_->cameras)
    {
        camera->SetAspectRatio(aspect_ratio);
    }
}

void RenderService::OnUpdate()
{
    // Debug menu
    if (input_service_->IsKeyPressed(GLFW_KEY_F2))
    {
        show_debug_menu_ = !show_debug_menu_;
    }

    glFrontFace(GL_CCW);

    if (debug_draw_camera_frustums_)
    {
        DrawCameraFrustums();
    }

    depth_pass_.Render();

    geometry_pass_.SetDepthMap(depth_pass_.GetDepthMap());
    geometry_pass_.SetLightSpaceTransformation(
        depth_pass_.GetLightSpaceTransformation());
    geometry_pass_.Render();
}

void RenderService::OnCleanup()
{
}

std::string_view RenderService::GetName() const
{
    return "RenderService";
}

void RenderService::OnGui()
{
    if (!show_debug_menu_)
    {
        return;
    }

    if (!ImGui::Begin("RenderService Debug", &show_debug_menu_))
    {
        ImGui::End();
        return;
    }

    ImGui::BeginTabBar("##RenderService Tabs");

    if (ImGui::BeginTabItem("General"))
    {
        if (ImGui::TreeNode("cameras", "Cameras: %zu",
                            render_data_->cameras.size()))
        {
            for (size_t i = 0; i < render_data_->cameras.size(); i++)
            {
                ImGui::TreeNode(
                    reinterpret_cast<void*>(i), "(%zu) %s", i,
                    render_data_->cameras[i]->GetEntity().GetName().c_str());
            }
            ImGui::TreePop();
        }

        ImGui::BulletText("Point Lights: %zu",
                          render_data_->point_lights.size());
        ImGui::BulletText("Entities: %zu", render_data_->entities.size());

        ImGui::Checkbox("Draw Camera Frustums", &debug_draw_camera_frustums_);

        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Depth Pass"))
    {
        depth_pass_.RenderDebugGui();
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Geometry Pass"))
    {
        geometry_pass_.RenderDebugGui();
        ImGui::EndTabItem();
    }

    ImGui::EndTabBar();
    ImGui::End();
}

DebugDrawList& RenderService::GetDebugDrawList()
{
    return debug_draw_list_;
}

void RenderService::DrawCameraFrustums()
{
    for (Camera* camera : render_data_->cameras)
    {
        if (!camera->IsDebugCamera())
        {
            debug_draw_list_.AddCuboid(camera->GetFrustumWorldVertices(),
                                       Color4u(0, 255, 0, 255));
        }
    }
}