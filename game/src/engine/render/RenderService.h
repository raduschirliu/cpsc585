#pragma once

#include <memory>
#include <object_ptr.hpp>
#include <vector>

#include "engine/fwd/FwdComponents.h"
#include "engine/fwd/FwdServices.h"
#include "engine/gui/OnGuiEvent.h"
#include "engine/render/DebugDrawList.h"
#include "engine/render/SceneRenderData.h"
#include "engine/render/passes/DepthPass.h"
#include "engine/render/passes/GeometryPass.h"
#include "engine/scene/Entity.h"
#include "engine/service/Service.h"

class DebugDrawList;

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
    void OnWindowSizeChanged(int width, int height) override;
    void OnUpdate() override;
    void OnCleanup() override;
    std::string_view GetName() const override;

    // From IEventSubscriber<OnGuiEvent>
    void OnGui() override;

    DebugDrawList& GetDebugDrawList();
    LaserMaterial& GetLaserMaterial();

  private:
    jss::object_ptr<InputService> input_service_;
    jss::object_ptr<AssetService> asset_service_;

    std::unique_ptr<SceneRenderData> render_data_;
    DepthPass depth_pass_;
    GeometryPass geometry_pass_;
    DebugDrawList debug_draw_list_;
    bool show_debug_menu_;
    bool debug_draw_camera_frustums_;

    void DrawCameraFrustums();
};
