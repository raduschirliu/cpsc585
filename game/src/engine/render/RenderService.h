#pragma once

#include <memory>
#include <object_ptr.hpp>
#include <vector>

#include "engine/core/gfx/ShaderProgram.h"
#include "engine/core/gfx/VertexArray.h"
#include "engine/core/gfx/VertexBuffer.h"
#include "engine/render/Renderable.h"
#include "engine/service/Service.h"
#include "engine/scene/Entity.h"

class Camera;

struct RenderData
{
    jss::object_ptr<const Entity> entity;
    VertexArray vertex_array;
    VertexBuffer vertex_buffer;
    ElementArrayBuffer element_buffer;
};

class RenderService final : public Service
{
  public:
    RenderService();

    void RegisterRenderable(const Entity& entity);
    void UnregisterRenderable(const Entity& entity);
    void RegisterCamera(const Camera& camera);

    // From Service
    void OnInit() override;
    void OnStart(ServiceProvider& service_provider) override;
    void OnUpdate() override;
    void OnCleanup() override;
    std::string_view GetName() const override;

  private:
    std::vector<std::unique_ptr<RenderData>> render_list_;
    std::vector<jss::object_ptr<const Camera>> cameras_;
    ShaderProgram shader_;

    void RenderPrepare();
    void RenderCameraView(const Camera& camera);
};
