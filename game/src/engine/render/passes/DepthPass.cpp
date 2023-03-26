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

struct MeshRenderData
{
    const Entity* entity;
    size_t index_count;
    RenderBuffers buffers;
};

static constexpr uint32_t kShadowMapWidth = 1024;
static constexpr uint32_t kShadowMapHeight = 1024;
static constexpr vec3 kLightPos(230.0f, 150.0f, 35.0f);

DepthPass::DepthPass(SceneRenderData& render_data)
    : render_data_(render_data),
      fbo_(),
      depth_map_(),
      shader_("resources/shaders/depth_map.vert",
              "resources/shaders/empty.frag"),
      meshes_{}
{
}

DepthPass::~DepthPass() = default;

void DepthPass::RegisterRenderable(const Entity& entity,
                                   const MeshRenderer& renderer)
{
    auto data = make_unique<MeshRenderData>();
    data->entity = &entity;

    // Configure vertex array/buffer and upload data
    data->buffers.vertex_array.Bind();
    data->buffers.vertex_buffer.Bind();
    data->buffers.element_buffer.Bind();

    data->buffers.vertex_buffer.ConfigureAttribute(
        0, 3, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, position));

    constexpr size_t index_size = sizeof(uint32_t);
    vector<Vertex> vertices;
    vector<uint32_t> indices;

    for (const auto& mesh : renderer.GetMeshes())
    {
        const uint32_t vertex_offset = static_cast<uint32_t>(vertices.size());
        const uint32_t index_offset = static_cast<uint32_t>(indices.size());
        const size_t index_count = mesh.mesh->indices.size();

        vertices.insert(vertices.end(), mesh.mesh->vertices.begin(),
                        mesh.mesh->vertices.end());
        indices.insert(indices.end(), mesh.mesh->indices.begin(),
                       mesh.mesh->indices.end());

        for (size_t i = 0; i < index_count; i++)
        {
            indices[index_offset + i] += vertex_offset;
        }
    }

    data->buffers.vertex_buffer.Upload(vertices, GL_STATIC_DRAW);
    data->buffers.element_buffer.Upload(indices, GL_STATIC_DRAW);
    data->index_count = indices.size();

    VertexArray::Unbind();

    // Add to render list
    meshes_.push_back(std::move(data));
}

void DepthPass::UnregisterRenderable(const Entity& entity)
{
    const uint32_t target_id = entity.GetId();
    std::erase_if(render_data_.entities, [target_id](const Entity* x)
                  { return x->GetId() == target_id; });
}

void DepthPass::Init()
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

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DepthPass::Render()
{
    glViewport(0, 0, kShadowMapWidth, kShadowMapHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glClear(GL_DEPTH_BUFFER_BIT);

    RenderMeshes();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DepthPass::ResetState()
{
    meshes_.clear();
}

const TextureHandle& DepthPass::GetDepthMap() const
{
    return depth_map_;
}

void DepthPass::RenderMeshes()
{
    const mat4 light_view =
        glm::lookAt(kLightPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    float near_plane = 1.0f, far_plane = 100.0f;
    const mat4 light_proj =
        glm::ortho(-25.0f, 25.0f, -25.0f, 25.0f, near_plane, far_plane);

    const mat4 light_space = light_proj * light_view;

    shader_.Use();
    shader_.SetUniform("uLightSpaceMatrix", light_space);

    // Render each object
    for (const auto& obj : meshes_)
    {
        const MeshRenderer& renderer =
            obj->entity->GetComponent<MeshRenderer>();
        const Transform& transform = obj->entity->GetComponent<Transform>();

        const mat4& model_matrix = transform.GetModelMatrix();

        // Vert shader vars
        shader_.SetUniform("uModelMatrix", model_matrix);

        obj->buffers.vertex_array.Bind();

        // Draw all meshes that are part of this object
        const GLsizei index_count = static_cast<GLsizei>(obj->index_count);
        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
    }
}