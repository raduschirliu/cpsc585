#pragma once

#include <memory>
#include <vector>

#include "engine/core/gfx/GLHandles.h"
#include "engine/core/gfx/ShaderProgram.h"
#include "engine/fwd/FwdComponents.h"
#include "engine/render/RenderBuffers.h"
#include "engine/render/SceneRenderData.h"

struct MeshRenderData;

class DepthPass
{
  public:
    DepthPass(SceneRenderData& render_data);
    ~DepthPass(); /* = default; */

    void RegisterRenderable(const Entity& entity, const MeshRenderer& renderer);
    void UnregisterRenderable(const Entity& entity);

    void Init();
    void Render();
    void ResetState();

    const TextureHandle& GetDepthMap() const;

  private:
    SceneRenderData& render_data_;
    FramebufferHandle fbo_;
    TextureHandle depth_map_;
    ShaderProgram shader_;
    std::vector<std::unique_ptr<MeshRenderData>> meshes_;

    void RenderMeshes();
};
