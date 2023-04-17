#include "engine/render/ParticleSystem.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#include <random>

#include "engine/asset/AssetService.h"
#include "engine/core/debug/Log.h"
#include "engine/core/gui/PropertyWidgets.h"
#include "engine/core/math/Random.h"
#include "engine/render/RenderService.h"
#include "engine/scene/Entity.h"
#include "engine/scene/Transform.h"

using glm::mat4;
using glm::vec3;
using glm::vec4;
using std::string;
using std::vector;

ParticleSystem::ParticleSystem(ParticleDrawList& draw_list,
                               const ParticleSystemProperties& properties)
    : draw_list_(draw_list),
      particles_{},
      properties_(properties)
{
}

void ParticleSystem::Update(const Timestep& delta_time)
{
    const float sec_delta = static_cast<float>(delta_time.GetSeconds());

    for (auto& particle : particles_)
    {
        // Ignore dead particles
        if (particle.lifetime <= 0.0f)
        {
            continue;
        }

        const float lifetime_t =
            1.0f - particle.lifetime / properties_.lifetime;

        // Update particle state
        particle.pos += particle.velocity * sec_delta;
        particle.color = glm::mix(properties_.color_start,
                                  properties_.color_end, lifetime_t);
        particle.size =
            glm::mix(properties_.size_start, properties_.size_end, lifetime_t);

        // Submit to GPU
        draw_list_.AddParticle(particle);

        // Update state for next render
        particle.velocity += properties_.acceleration * sec_delta;
        particle.lifetime -= sec_delta;
    }
}

void ParticleSystem::Emit(const glm::vec3& pos)
{
    for (uint32_t i = 0; i < properties_.burst_amount; i++)
    {
        const vec3 velocity = glm::sphericalRand(1.0f) * properties_.speed;

        Particle& next_particle = NextParticle();
        next_particle = Particle{
            .pos = pos,
            .velocity = velocity,
            .color = properties_.color_start,
            .texture = properties_.texture,
            .size = properties_.size_start,
            .lifetime = properties_.lifetime,
            .angle_rad = 0.0f,
            .camera_distance = 0.0f,
        };
    }
}

Particle& ParticleSystem::NextParticle()
{
    for (auto iter = particles_.begin(); iter != particles_.end(); iter++)
    {
        if (iter->lifetime <= 0.0f)
        {
            return *iter;
        }
    }

    particles_.push_back(Particle{});
    return particles_.back();
}

void ParticleSystem::SetProperties(const ParticleSystemProperties& properties)
{
    properties_ = properties;
}
