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
    void RenderDebugGui();
    void ResetState();

    void SetDrawDebugBounds(bool state);

    const TextureHandle& GetDepthMap() const;
    glm::mat4 GetLightSpaceTransformation() const;

  private:
    SceneRenderData& render_data_;
    FramebufferHandle fbo_;
    TextureHandle depth_map_;
    ShaderProgram shader_;
    std::vector<std::unique_ptr<MeshRenderData>> meshes_;
    bool debug_draw_bounds_;
    Transform* target_transform_;
    glm::vec3 target_pos_;
    glm::vec3 source_pos_;

    void RenderMeshes();
};
