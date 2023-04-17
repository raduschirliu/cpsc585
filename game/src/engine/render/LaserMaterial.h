#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "engine/core/gfx/Buffer.h"
#include "engine/core/gfx/ShaderProgram.h"
#include "engine/core/gfx/VertexArray.h"
#include "engine/core/gfx/VertexBuffer.h"
#include "engine/core/math/Quad.h"
#include "engine/fwd/FwdComponents.h"
#include "engine/fwd/FwdServices.h"
#include "engine/render/SceneRenderData.h"

class Texture;
struct CameraView;

struct LaserVertex
{
    glm::vec3 pos;
    glm::vec2 uv;
    float alpha;

    LaserVertex() : pos(0.0f, 0.0f, 0.0f), uv(0.0f, 0.0f), alpha(0.0f)
    {
    }

    LaserVertex(const glm::vec3& pos, const glm::vec2& uv, const float& alpha)
        : pos(pos),
          uv(uv),
          alpha(alpha)
    {
    }
};

class LaserMaterial
{
  public:
    LaserMaterial(const SceneRenderData& render_data_);

    void AddQuad(const Quad<LaserVertex>& quad);

    void LoadAssets(AssetService& asset_service);
    void Prepare();
    void Render(const CameraView& camera);
    void Clear();
    void RecompileShader();

  private:
    const SceneRenderData& render_data_;
    const Texture* texture_;

    VertexArray vertex_array_;
    VertexBuffer vertex_buffer_;
    ElementArrayBuffer element_buffer_;
    ShaderProgram shader_;
    std::vector<Quad<LaserVertex>> quads_;
    std::vector<uint32_t> indices_;
};
