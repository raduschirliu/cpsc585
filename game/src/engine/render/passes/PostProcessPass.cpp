#include "engine/render/passes/PostProcessPass.h"

#include <imgui.h>

#include <vector>

#include "engine/core/gfx/Texture.h"

using glm::vec2;
using std::vector;

struct ScreenVertex
{
    vec2 pos;
    vec2 uv;
};

const vector<ScreenVertex> kScreenQuadVertices = {
    ScreenVertex(vec2(-1.0f, 1.0f), vec2(0.0f, 1.0f)),
    ScreenVertex(vec2(-1.0f, -1.0f), vec2(0.0f, 0.0f)),
    ScreenVertex(vec2(1.0f, -1.0f), vec2(1.0f, 0.0f)),

    ScreenVertex(vec2(-1.0f, 1.0f), vec2(0.0f, 1.0f)),
    ScreenVertex(vec2(1.0f, -1.0f), vec2(1.0f, 0.0f)),
    ScreenVertex(vec2(1.0f, 1.0f), vec2(1.0f, 1.0f)),
};

PostProcessPass::PostProcessPass(SceneRenderData& render_data,
                                 TextureHandle& screen_texture)
    : shader_("resources/shaders/post_process.vert",
              "resources/shaders/post_process.frag"),
      screen_texture_(screen_texture),
      render_data_(render_data),
      quad_vao_(),
      quad_vbo_(),
      gamma_(2.2f),
      exposure_(1.0f)
{
}

void PostProcessPass::Init()
{
    quad_vao_.Bind();

    const size_t stride = sizeof(ScreenVertex);
    const size_t pos_offset = offsetof(ScreenVertex, pos);
    const size_t uv_offset = offsetof(ScreenVertex, uv);

    quad_vbo_.Bind();
    quad_vbo_.ConfigureAttribute(0, 2, GL_FLOAT, stride, pos_offset);
    quad_vbo_.ConfigureAttribute(1, 2, GL_FLOAT, stride, uv_offset);
    quad_vbo_.Upload(kScreenQuadVertices, GL_STATIC_DRAW);
}

void PostProcessPass::Render()
{
    // Render to default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, render_data_.screen_size.x, render_data_.screen_size.y);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_FRAMEBUFFER_SRGB);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    quad_vao_.Bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screen_texture_);

    shader_.Use();
    shader_.SetUniform("uScreenTexture", 0);
    shader_.SetUniform("uGamma", gamma_);
    shader_.SetUniform("uExposure", exposure_);

    const GLsizei vertex_count =
        static_cast<GLsizei>(kScreenQuadVertices.size());
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
}

void PostProcessPass::RenderDebugGui()
{
    ImGui::DragFloat("Gamma", &gamma_, 0.1f, 0.0f, 10.0f);
    ImGui::DragFloat("Exposure", &exposure_, 0.1f, 0.0f, 10.0f);
}
