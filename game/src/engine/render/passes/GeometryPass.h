#pragma once

#include <memory>
#include <vector>

#include "engine/core/gfx/ShaderProgram.h"
#include "engine/core/gfx/VertexArray.h"
#include "engine/core/gfx/VertexBuffer.h"
#include "engine/fwd/FwdComponents.h"
#include "engine/render/DebugDrawList.h"
#include "engine/render/SceneRenderData.h"

struct CameraView;
struct MeshRenderData;
class Cubemap;

struct RenderBuffers
{
    VertexArray vertex_array;
    VertexBuffer vertex_buffer;
    ElementArrayBuffer element_buffer;
};

class GeometryPass
{
  public:
    GeometryPass(SceneRenderData& render_data);
    ~GeometryPass(); /* = default; (in cpp) */

    void RegisterRenderable(const Entity& entity, const MeshRenderer& renderer);
    void UnregisterRenderable(const Entity& entity);

    void Init();
    void Render();
    void ResetState();

    void SetWireframe(bool state);
    DebugDrawList& GetDebugDrawList();

  private:
    SceneRenderData& render_data_;
    std::vector<std::unique_ptr<MeshRenderData>> meshes_;
    ShaderProgram shader_, debug_shader_, skybox_shader_;
    RenderBuffers skybox_buffers_;
    const Cubemap* skybox_texture_;
    DebugDrawList debug_draw_list_;
    bool wireframe_;

    CameraView PrepareCameraView(Camera& camera);
    void RenderMeshes(const CameraView& camera);
    void RenderDebugDrawList(const CameraView& camera);
    void RenderSkybox(const CameraView& camera);
};
