#include "engine/render/ParticleDrawList.h"

#include <fmt/format.h>

#include <algorithm>

#include "engine/core/gfx/Texture.h"
#include "engine/render/Camera.h"
#include "engine/scene/Transform.h"

using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using std::vector;

struct BillboardVertex
{
    vec3 pos;
    vec2 uv;
    BillboardVertex(const vec3& pos, const vec2& uv): pos(pos), uv(uv) {}
};

static constexpr size_t kDefaultBufferSize = sizeof(GpuParticle) * 10;
static int kNextTextureId = 0;

const vector<BillboardVertex> kQuadVertices = {
    BillboardVertex(vec3(-0.5f, 0.5f, 0.0f), vec2(0.0f, 1.0f)),
    BillboardVertex(vec3(-0.5f, -0.5f, 0.0f), vec2(0.0f, 0.0f)),
    BillboardVertex(vec3(0.5f, -0.5f, 0.0f), vec2(1.0f, 0.0f)),

    BillboardVertex(vec3(-0.5f, 0.5f, 0.0f), vec2(0.0f, 1.0f)),
    BillboardVertex(vec3(0.5f, -0.5f, 0.0f), vec2(1.0f, 0.0f)),
    BillboardVertex(vec3(0.5f, 0.5f, 0.0f), vec2(1.0f, 1.0f)),
};

ParticleDrawList::ParticleDrawList()
    : shader_("resources/shaders/particle.vert",
              "resources/shaders/particle.frag"),
      vertex_array_(),
      vertex_buffer_(),
      quad_buffer_(),
      particles_(),
      gpu_particles_()
{
}

void ParticleDrawList::AddParticle(const Particle& particle)
{
    if (particle.lifetime <= 0.0f)
    {
        return;
    }

    particles_.push_back(particle);
}

void ParticleDrawList::Init()
{
    vertex_array_.Bind();

    // Per-vertex attributes
    vertex_buffer_.Bind();

    constexpr size_t vertex_size = sizeof(BillboardVertex);
    constexpr size_t vertex_pos_offset = offsetof(BillboardVertex, pos);
    constexpr size_t vertex_uv_offset = offsetof(BillboardVertex, uv);

    // Pos
    vertex_buffer_.ConfigureAttribute(0, 3, GL_FLOAT, vertex_size,
                                      vertex_pos_offset);
    // Uv
    vertex_buffer_.ConfigureAttribute(1, 2, GL_FLOAT, vertex_size,
                                      vertex_uv_offset);

    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 0);

    vertex_buffer_.Upload(kQuadVertices, GL_STATIC_DRAW);

    // Per-quad attributes
    quad_buffer_.Bind();

    constexpr size_t quad_size = sizeof(GpuParticle);
    constexpr size_t quad_color_offset = offsetof(GpuParticle, color);
    constexpr size_t quad_model_matrix_offset =
        offsetof(GpuParticle, model_matrix);
    constexpr size_t quad_texture_index_offset =
        offsetof(GpuParticle, texture_index);
    constexpr size_t vec4_size = sizeof(vec4);

    // Color
    quad_buffer_.ConfigureAttribute(2, 4, GL_FLOAT, quad_size,
                                    quad_color_offset);
    // Model Matrix
    quad_buffer_.ConfigureAttribute(3, 4, GL_FLOAT, quad_size,
                                    quad_model_matrix_offset + vec4_size * 0);
    quad_buffer_.ConfigureAttribute(4, 4, GL_FLOAT, quad_size,
                                    quad_model_matrix_offset + vec4_size * 1);
    quad_buffer_.ConfigureAttribute(5, 4, GL_FLOAT, quad_size,
                                    quad_model_matrix_offset + vec4_size * 2);
    quad_buffer_.ConfigureAttribute(6, 4, GL_FLOAT, quad_size,
                                    quad_model_matrix_offset + vec4_size * 3);

    // Texture Index
    quad_buffer_.ConfigureIntAttribute(7, 4, GL_INT, quad_size,
                                       quad_texture_index_offset);

    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    glVertexAttribDivisor(7, 1);

    quad_buffer_.Allocate(kDefaultBufferSize, GL_STATIC_DRAW);
}

void ParticleDrawList::Prepare(const CameraView& camera)
{
    for (auto& particle : particles_)
    {
        // find camera distance
        particle.camera_distance = glm::distance2(camera.pos, particle.pos);
    }

    // Sort in reverse order - far particles first, close particles last
    std::sort(particles_.begin(), particles_.end(),
              [](const Particle& a, const Particle& b)
              { return a.camera_distance > b.camera_distance; });

    // Copy to GPU
    for (auto& particle : particles_)
    {
        // TODO(radu): take particle's own rotation into account too...
        // rotate around the fwd (z) axis

        const vec3 up_dir(0.0f, 1.0f, 0.0f);
        const vec3 f_dir(glm::normalize(camera.pos - particle.pos));
        const vec3 s_dir(glm::normalize(glm::cross(f_dir, up_dir)));
        const vec3 u_dir(glm::cross(s_dir, f_dir));

        mat4 model_matrix(1.0f);

        // Setting the columns to the 3 basis vectors
        model_matrix[0] = vec4(s_dir, 0.0f);
        model_matrix[1] = vec4(u_dir, 0.0f);
        model_matrix[2] = vec4(f_dir, 0.0f);

        // Translation
        model_matrix[3] = vec4(particle.pos, 1.0f);

        // Scaling
        model_matrix = glm::scale(model_matrix, vec3(particle.size));

        gpu_particles_.push_back(GpuParticle{
            .color = particle.color,
            .model_matrix = model_matrix,
            .texture_index = GetTextureIndex(particle.texture),
        });
    }

    quad_buffer_.ResizeToFit(gpu_particles_);
    quad_buffer_.UploadSubset(gpu_particles_, 0);
}

void ParticleDrawList::Render(const CameraView& camera)
{
    vertex_array_.Bind();

    // Bind textures and shader
    shader_.Use();
    shader_.SetUniform("uViewProjMatrix", camera.view_proj_matrix);

    for (auto& entry : textures_)
    {
        entry.first->Bind(entry.second);
        std::string uniform_name = fmt::format("uTextures[{}]", entry.second);
        shader_.SetUniform(uniform_name, entry.second);
    }

    // Render
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const GLsizei vertex_count = static_cast<GLsizei>(kQuadVertices.size());
    const GLsizei particle_count = static_cast<GLsizei>(gpu_particles_.size());
    glDrawArraysInstanced(GL_TRIANGLES, 0, vertex_count, particle_count);

    glDisable(GL_BLEND);
}

void ParticleDrawList::Clear()
{
    particles_.clear();
    gpu_particles_.clear();
}

int ParticleDrawList::GetTextureIndex(const Texture* texture)
{
    int index = 0;
    auto iter = textures_.find(texture);

    if (iter == textures_.end())
    {
        index = kNextTextureId;
        textures_.emplace(texture, kNextTextureId);
        kNextTextureId += 1;
    }
    else
    {
        index = iter->second;
    }

    return index;
}
