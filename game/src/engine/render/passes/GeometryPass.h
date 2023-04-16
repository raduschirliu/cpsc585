#pragma once

#include <memory>
#include <vector>

#include "engine/core/gfx/GLHandles.h"
#include "engine/core/gfx/ShaderProgram.h"
#include "engine/fwd/FwdComponents.h"
#include "engine/render/DebugDrawList.h"
#include "engine/render/LaserMaterial.h"
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
    LaserMaterial& GetLaserMaterial();
    TextureHandle& GetScreenTexture();

  private:
    FramebufferHandle fbo_;
    RenderbufferHandle rbo_;
    TextureHandle screen_texture_;
    SceneRenderData& render_data_;
    const std::vector<std::unique_ptr<ShadowMap>>& shadow_maps_;
    std::vector<std::unique_ptr<MeshRenderData>> meshes_;
    ShaderProgram shader_, debug_shader_, skybox_shader_;
    LaserMaterial laser_material_;
    RenderBuffers skybox_buffers_;
    const Cubemap* skybox_texture_;
    bool wireframe_;
    float min_shadow_bias_;
    float max_shadow_bias_;
    size_t debug_num_draw_calls_;
    size_t debug_total_buffer_size_;
    glm::ivec2 last_screen_size_;

    void InitSkybox();
    void InitFbo();
    void CheckScreenResize();
    CameraView PrepareCameraView(Camera& camera);
    void RenderMeshes(const CameraView& camera);
    void RenderDebugDrawList(const CameraView& camera);
    void RenderSkybox(const CameraView& camera);
    void RenderParticles(const Camera& camera);
    void CreateBuffers(const Entity& entity, const MeshRenderer& renderer);
};
