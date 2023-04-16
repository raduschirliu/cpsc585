#include "engine/render/ParticleDrawList.h"

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
};

static constexpr size_t kDefaultBufferSize = sizeof(GpuParticle) * 10;

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
      quad_buffer_(),
      vertex_buffer_(),
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

    // Quad buffer
    quad_buffer_.Bind();

    constexpr size_t quad_size = sizeof(BillboardVertex);
    constexpr size_t quad_pos_offset = offsetof(BillboardVertex, pos);
    constexpr size_t quad_uv_offset = offsetof(BillboardVertex, uv);

    // Pos
    quad_buffer_.ConfigureAttribute(0, 3, GL_FLOAT, quad_size, quad_pos_offset);
    // Uv
    quad_buffer_.ConfigureAttribute(1, 2, GL_FLOAT, quad_size, quad_uv_offset);

    quad_buffer_.AttributeDivisor(0, 0);
    quad_buffer_.AttributeDivisor(1, 0);

    quad_buffer_.Upload(kQuadVertices, GL_STATIC_DRAW);

    // Vertex data buffer
    vertex_buffer_.Bind();

    constexpr size_t vertex_size = sizeof(GpuParticle);
    constexpr size_t vertex_color_offset = offsetof(GpuParticle, color);
    constexpr size_t vertex_model_matrix_offset =
        offsetof(GpuParticle, model_matrix);
    constexpr size_t vertex_texture_index_offset =
        offsetof(GpuParticle, texture_index);
    constexpr size_t vec4_size = sizeof(vec4);

    // Color
    vertex_buffer_.ConfigureAttribute(2, 4, GL_FLOAT, vertex_size,
                                      vertex_color_offset);
    // Model Matrix
    vertex_buffer_.ConfigureAttribute(
        3, 4, GL_FLOAT, vertex_size,
        vertex_model_matrix_offset + vec4_size * 0);
    vertex_buffer_.ConfigureAttribute(
        4, 4, GL_FLOAT, vertex_size,
        vertex_model_matrix_offset + vec4_size * 1);
    vertex_buffer_.ConfigureAttribute(
        5, 4, GL_FLOAT, vertex_size,
        vertex_model_matrix_offset + vec4_size * 2);
    vertex_buffer_.ConfigureAttribute(
        6, 4, GL_FLOAT, vertex_size,
        vertex_model_matrix_offset + vec4_size * 3);

    // Texture Index
    vertex_buffer_.ConfigureAttribute(7, 4, GL_INT, vertex_size,
                                      vertex_texture_index_offset);

    vertex_buffer_.AttributeDivisor(3, 1);
    vertex_buffer_.AttributeDivisor(4, 1);
    vertex_buffer_.AttributeDivisor(5, 1);
    vertex_buffer_.AttributeDivisor(6, 1);
    vertex_buffer_.AttributeDivisor(7, 1);

    vertex_buffer_.Allocate(kDefaultBufferSize, GL_STATIC_DRAW);
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
        // TODO(radu): take rotation into account too
        // TODO(radu): rotate particle to face camera
        mat4 model_matrix = glm::translate(mat4(1.0f), particle.pos);

        gpu_particles_.push_back(GpuParticle{
            .color = particle.color,
            .model_matrix = model_matrix,
            .texture_index = 0,
        });
    }

    vertex_buffer_.ResizeToFit(gpu_particles_);
    vertex_buffer_.UploadSubset(gpu_particles_, 0);
}

void ParticleDrawList::Render(const CameraView& camera)
{
    vertex_array_.Bind();

    shader_.Use();
    shader_.SetUniform("uViewProjMatrix", camera.view_proj_matrix);

    glDisable(GL_CULL_FACE);

    const GLsizei vertex_count = static_cast<GLsizei>(kQuadVertices.size());
    const GLsizei particle_count = static_cast<GLsizei>(gpu_particles_.size());
    glDrawArraysInstanced(GL_TRIANGLES, 0, vertex_count, particle_count);
}

void ParticleDrawList::Clear()
{
    particles_.clear();
    gpu_particles_.clear();
}
