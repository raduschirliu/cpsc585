#include "engine/render/ParticleSystem.h"

#include <imgui.h>

#include <glm/gtc/matrix_transform.hpp>

#include "engine/asset/AssetService.h"
#include "engine/core/debug/Log.h"
#include "engine/core/gui/PropertyWidgets.h"
#include "engine/render/RenderService.h"
#include "engine/scene/Entity.h"
#include "engine/scene/Transform.h"

using glm::mat4;
using glm::vec3;
using glm::vec4;
using std::string;
using std::vector;

void ParticleSystem::OnInit(const ServiceProvider& service_provider)
{
    // Services
    render_service_ = &service_provider.GetService<RenderService>();
    asset_service_ = &service_provider.GetService<AssetService>();

    // Components
    transform_ = &GetEntity().GetComponent<Transform>();
}

void ParticleSystem::OnUpdate(const Timestep& delta_time)
{
    for (auto& particle : particles_)
    {
        // Update particle transform
        particle.pos += particle.velocity;

        // Submit to GPU
        render_service_->GetParticleDrawList().AddParticle(particle);

        // Update lifetime
        if (particle.lifetime > 0.0f)
        {
            const float sec_delta = static_cast<float>(delta_time.GetSeconds());
            particle.lifetime -= sec_delta;
        }
    }
}

void ParticleSystem::OnDebugGui()
{
}

void ParticleSystem::OnDestroy()
{
}

std::string_view ParticleSystem::GetName() const
{
    return "ParticleSystem";
}

void ParticleSystem::Emit(const vec3& pos)
{
    particles_.push_back(Particle{
        .pos = transform_->GetPosition() + pos,
        .velocity = vec3(0.0f, -1.0f, 0.0f),
        .color = vec4(1.0f, 0.0f, 0.0f, 1.0f),
        .texture = nullptr,
        .size = 5.0f,
        .lifetime = 6.0f,
        .angle_rad = 0.0f,
        .camera_distance = 0.0f,
    });
}