#pragma once

#include "engine/core/gfx/GLHandles.h"
#include "engine/core/gfx/ShaderProgram.h"
#include "engine/core/gfx/VertexArray.h"
#include "engine/core/gfx/VertexBuffer.h"
#include "engine/render/SceneRenderData.h"

class PostProcessPass
{
  public:
    PostProcessPass(SceneRenderData& render_data,
                    TextureHandle& screen_texture_);

    void Init();
    void Render();
    void RenderDebugGui();

  private:
    ShaderProgram shader_;
    TextureHandle& screen_texture_;
    SceneRenderData& render_data_;
    VertexArray quad_vao_;
    VertexBuffer quad_vbo_;
};
