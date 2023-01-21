#pragma once

#include <memory>
#include <object_ptr.hpp>
#include <vector>

#include "engine/core/gfx/ShaderProgram.h"
#include "engine/core/gfx/VertexArray.h"
#include "engine/core/gfx/VertexBuffer.h"
#include "engine/render/Renderable.h"
#include "engine/service/Service.h"

struct RenderData
{
    jss::object_ptr<const RenderableComponent> renderable;
    VertexArray vertex_array;
    VertexBuffer vertex_buffer;
};

class RenderService final : public Service
{
  public:
    RenderService();

    void RegisterRenderable(const RenderableComponent& renderable);

    // From Service
    void OnInit() override;
    void OnStart(ServiceProvider& service_provider) override;
    void OnUpdate() override;
    void OnCleanup() override;
    std::string_view GetName() const override;

  protected:
    void RenderPrepare();

  private:
    std::vector<std::unique_ptr<RenderData>> render_list_;
    ShaderProgram shader_;
};
