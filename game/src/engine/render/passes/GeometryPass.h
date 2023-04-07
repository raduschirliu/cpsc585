#pragma once

#include <memory>
#include <vector>

#include "engine/core/gfx/ShaderProgram.h"
#include "engine/fwd/FwdComponents.h"
#include "engine/render/DebugDrawList.h"
#include "engine/render/RenderBuffers.h"
#include "engine/render/SceneRenderData.h"

struct CameraView;
struct MeshRenderData;
class Cubemap;
class ShadowMap;

class GeometryPass
{
  public:
    GeometryPass(SceneRenderData& render_data,
                 const std::vector<std::unique_ptr<ShadowMap>>& shadow_maps);
    ~GeometryPass(); /* = default; (in cpp) */

    void RegisterRenderable(const Entity& entity, const MeshRenderer& renderer);
    void UnregisterRenderable(const Entity& entity);

    void Init();
    void Render();
    void RenderDebugGui();
    void ResetState();

    void SetWireframe(bool state);

  private:
    SceneRenderData& render_data_;
    const std::vector<std::unique_ptr<ShadowMap>>& shadow_maps_;
    std::vector<std::unique_ptr<MeshRenderData>> meshes_;
    ShaderProgram shader_, debug_shader_, skybox_shader_;
    RenderBuffers skybox_buffers_;
    const Cubemap* skybox_texture_;
    bool wireframe_;
    float min_shadow_bias_;
    float max_shadow_bias_;

    CameraView PrepareCameraView(Camera& camera);
    void RenderMeshes(const CameraView& camera);
    void RenderDebugDrawList(const CameraView& camera);
    void RenderSkybox(const CameraView& camera);
};
