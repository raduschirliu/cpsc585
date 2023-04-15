#include "engine/render/LaserMaterial.h"

#include "engine/asset/AssetService.h"
#include "engine/core/gfx/Texture.h"
#include "engine/render/Camera.h"

using glm::mat4;

LaserMaterial::LaserMaterial(const SceneRenderData& render_data)
    : render_data_(render_data),
      texture_(nullptr),
      vertex_array_(),
      vertex_buffer_(),
      element_buffer_(),
      shader_("resources/shaders/laser.vert", "resources/shaders/laser.frag"),
      quads_{}
{
    constexpr GLsizei stride = sizeof(LaserVertex);
    constexpr GLsizei pos_offset = offsetof(LaserVertex, pos);
    constexpr GLsizei uv_offset = offsetof(LaserVertex, uv);

    vertex_array_.Bind();
    vertex_buffer_.Bind();
    element_buffer_.Bind();

    // Position
    vertex_buffer_.ConfigureAttribute(0, 3, GL_FLOAT, stride, pos_offset);
    // UV
    vertex_buffer_.ConfigureAttribute(1, 2, GL_FLOAT, stride, uv_offset);
}

void LaserMaterial::AddQuad(const Quad<LaserVertex>& quad)
{
    quads_.push_back(quad);

    const uint32_t start_index = indices_.size();
    indices_.insert(
        indices_.end(),
        {
            start_index, start_index + 1, start_index + 2,  // Triangle 1
            start_index, start_index + 2, start_index + 3,  // Triangle 2
        });
}

void LaserMaterial::LoadAssets(AssetService& asset_service)
{
    texture_ = &asset_service.GetTexture("laser_mask");
}

void LaserMaterial::Prepare()
{
    // TODO(radu): OPTIMIZATION - Would be better to allocate buffer at start
    // and only upload to part of the buffer instead
    vertex_buffer_.Upload(quads_, GL_DYNAMIC_DRAW);
    element_buffer_.Upload(indices_, GL_DYNAMIC_DRAW);
}

void LaserMaterial::Draw(const Camera& camera)
{
    if (indices_.size() == 0)
    {
        return;
    }

    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    vertex_array_.Bind();
    texture_->Bind(0);

    const mat4 view_proj =
        camera.GetProjectionMatrix() * camera.GetViewMatrix();
    const mat4 model = mat4(1.0f);

    shader_.Use();
    shader_.SetUniform("uMask", 0);
    shader_.SetUniform("uTime", static_cast<float>(render_data_.total_time));
    shader_.SetUniform("uViewProjMatrix", view_proj);
    shader_.SetUniform("uModelMatrix", model);

    const GLsizei index_count = static_cast<GLsizei>(indices_.size());
    glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);

    glDisable(GL_BLEND);
}

void LaserMaterial::Clear()
{
    quads_.clear();
    indices_.clear();
}

void LaserMaterial::RecompileShader()
{
    shader_.Recompile();
}
