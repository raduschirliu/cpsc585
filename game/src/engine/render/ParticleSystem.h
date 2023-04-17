#pragma once

#include <object_ptr.hpp>
#include <optional>
#include <string>
#include <vector>

#include "engine/core/math/Timestep.h"
#include "engine/fwd/FwdComponents.h"
#include "engine/fwd/FwdServices.h"
#include "engine/render/ParticleDrawList.h"

struct ParticleSystemProperties
{
    glm::vec3 acceleration;
    Color4 color_start;
    Color4 color_end;
    bool random_velocity;
    glm::vec3 velocity;
    float speed;
    float size_start;
    float size_end;
    float lifetime;
    const Texture* texture;
    uint32_t burst_amount;
};

class ParticleSystem
{
  public:
    ParticleSystem(ParticleDrawList& draw_list,
                   const ParticleSystemProperties& properties);

    void Update(const Timestep& delta_time);
    void Emit(const glm::vec3& pos);

    void SetProperties(const ParticleSystemProperties& properties);
    ParticleSystemProperties& GetProperties();

  private:
    ParticleDrawList& draw_list_;
    std::vector<Particle> particles_;
    ParticleSystemProperties properties_;

    Particle& NextParticle();
};
