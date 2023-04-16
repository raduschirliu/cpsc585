#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "engine/core/Colors.h"
#include "engine/core/gfx/Buffer.h"
#include "engine/core/gfx/ShaderProgram.h"
#include "engine/core/gfx/VertexArray.h"
#include "engine/core/gfx/VertexBuffer.h"

class Texture;
class Camera;

struct Particle
{
    glm::vec3 pos;
    glm::vec3 velocity;
    Color4 color;
    Texture* texture;
    float size;
    float lifetime;
    float angle_rad;
    float camera_distance;
};

struct GpuParticle
{
    Color4 color;
    glm::mat4 model_matrix;
    int texture_index;
};

struct CameraView;

class ParticleDrawList
{
  public:
    ParticleDrawList();

    void AddParticle(const Particle& particle);

    void Init();
    void Prepare(const CameraView& camera);
    void Render(const CameraView& camera);
    void Clear();

  private:
    ShaderProgram shader_;
    VertexArray vertex_array_;
    VertexBuffer quad_buffer_;
    VertexBuffer vertex_buffer_;
    std::vector<Particle> particles_;
    std::vector<GpuParticle> gpu_particles_;
};
