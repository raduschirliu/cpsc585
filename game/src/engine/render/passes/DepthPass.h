#pragma once

#include "engine/core/gfx/GLHandles.h"
#include "engine/core/gfx/ShaderProgram.h"
#include "engine/fwd/FwdComponents.h"
#include "engine/render/RenderBuffers.h"
#include "engine/render/SceneRenderData.h"

class DepthPass
{
  public:
    DepthPass(SceneRenderData& render_data);

    void RegisterRenderable(const Entity& entity, const MeshRenderer& renderer);
    void UnregisterRenderable(const Entity& entity);

    void Init();
    void Render();
    void ResetState();

  private:
    SceneRenderData& render_data_;
    FramebufferHandle fbo_;
    TextureHandle depth_map_;
    ShaderProgram shader_;
};
