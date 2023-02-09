#pragma once

#include <memory>
#include <object_ptr.hpp>
#include <vector>

#include "engine/core/gfx/ShaderProgram.h"
#include "engine/core/gfx/VertexArray.h"
#include "engine/core/gfx/VertexBuffer.h"
#include "engine/gui/OnGuiEvent.h"
#include "engine/scene/Entity.h"
#include "engine/service/Service.h"

class Camera;
class InputService;

struct RenderData
{
    jss::object_ptr<const Entity> entity;
    VertexArray vertex_array;
    VertexBuffer vertex_buffer;
    ElementArrayBuffer element_buffer;
};

class RenderService final : public Service, public IEventSubscriber<OnGuiEvent>
{
  public:
    RenderService();

    void RegisterRenderable(const Entity& entity);
    void UnregisterRenderable(const Entity& entity);
    void RegisterCamera(Camera& camera);
    void RegisterLight(Entity& entity);

    // From Service
    void OnInit() override;
    void OnStart(ServiceProvider& service_provider) override;
    void OnUpdate() override;
    void OnCleanup() override;
    std::string_view GetName() const override;

    // From IEventSubscriber<OnGuiEvent>
    void OnGui() override;

  private:
    jss::object_ptr<InputService> input_service_;
    std::vector<std::unique_ptr<RenderData>> render_list_;
    std::vector<jss::object_ptr<Camera>> cameras_;
    std::vector<jss::object_ptr<Entity>> lights_;
    ShaderProgram shader_;
    bool wireframe_;
    bool menu_open_;

    void RenderPrepare();
    void RenderCameraView(Camera& camera);
};
