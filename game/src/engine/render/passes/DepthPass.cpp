#include "engine/render/passes/DepthPass.h"

#include "engine/core/debug/Assert.h"
#include "engine/core/debug/Log.h"
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

static constexpr uint32_t kShadowMapWidth = 1024;
static constexpr uint32_t kShadowMapHeight = 1024;

DepthPass::DepthPass(SceneRenderData& render_data)
    : render_data_(render_data),
      fbo_(),
      depth_map_(),
      shader_("resources/shaders/depth_map.vert",
              "resources/shaders/empty.frag")
{
    // Create and attach depth map texture to FBO
    glBindTexture(GL_TEXTURE_2D, depth_map_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, kShadowMapWidth,
                 kShadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                           depth_map_, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
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
    glViewport(0, 0, kShadowMapWidth, kShadowMapHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void DepthPass::ResetState()
{
}