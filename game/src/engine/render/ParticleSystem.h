#pragma once

#include <object_ptr.hpp>
#include <optional>
#include <string>
#include <vector>

#include "engine/fwd/FwdComponents.h"
#include "engine/fwd/FwdServices.h"
#include "engine/render/ParticleDrawList.h"
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"

class ParticleSystem final : public Component,
                             public IEventSubscriber<OnUpdateEvent>
{
  public:
    ParticleSystem() = default;

    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    void OnDebugGui() override;
    void OnDestroy() override;
    std::string_view GetName() const override;

    // From IEventSubscriber<OnUpdateEvent>
    void OnUpdate(const Timestep& delta_time) override;

    void Emit(const glm::vec3& pos);

  private:
    jss::object_ptr<RenderService> render_service_;
    jss::object_ptr<AssetService> asset_service_;
    jss::object_ptr<Transform> transform_;

    std::vector<Particle> particles_;
};
