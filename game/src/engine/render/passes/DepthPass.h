#pragma once

#include <memory>
#include <vector>

#include "engine/core/gfx/GLHandles.h"
#include "engine/core/gfx/ShaderProgram.h"
#include "engine/fwd/FwdComponents.h"
#include "engine/render/RenderBuffers.h"
#include "engine/render/SceneRenderData.h"

struct MeshRenderData;
class ShadowMap;

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

    const std::vector<std::unique_ptr<ShadowMap>>& GetShadowMaps() const;

  private:
    SceneRenderData& render_data_;
    std::vector<std::unique_ptr<ShadowMap>> shadow_maps_;
    ShaderProgram shader_;
    std::vector<std::unique_ptr<MeshRenderData>> meshes_;
    bool debug_draw_shadow_bounds_;
    bool debug_draw_camera_bounds_;
    Camera* current_camera_;

    bool ShouldRun();

    void RenderShadowMaps();
    void RenderMeshes(ShadowMap& shadow_map);
    void RenderDebugCameraBounds(ShadowMap& shadow_map);
    void RenderDebugShadowBounds(ShadowMap& shadow_map);
};
