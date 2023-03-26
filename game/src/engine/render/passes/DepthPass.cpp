#include "engine/render/passes/DepthPass.h"

#include "engine/core/debug/Assert.h"
#include "engine/core/debug/Log.h"
#include "engine/core/gfx/Cubemap.h"
#include "engine/core/gfx/ShaderProgram.h"
#include "engine/core/gfx/VertexArray.h"
#include "engine/core/gfx/VertexBuffer.h"
#include "engine/render/Camera.h"
#include "engine/render/MeshRenderer.h"
#include "engine/scene/Entity.h"

using glm::ivec2;
using glm::mat4;
using glm::vec3;
using std::make_unique;
using std::vector;

DepthPass::DepthPass(SceneRenderData& render_data) : render_data_(render_data)
{
}

void DepthPass::RegisterRenderable(const Entity& entity,
                                   const MeshRenderer& renderer)
{
}

void DepthPass::UnregisterRenderable(const Entity& entity)
{
}

void DepthPass::Init()
{
}

void DepthPass::Render()
{
}

void DepthPass::ResetState()
{
}