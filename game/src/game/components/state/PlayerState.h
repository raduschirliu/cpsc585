#pragma once

#include "PlayerStateStruct.h"
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"

class PlayerState : public Component, public IEventSubscriber<OnUpdateEvent>
{
  public:
    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    void OnUpdate(const Timestep& delta_time) override;
    std::string_view GetName() const override;

  private:
    PlayerStateStruct player_state;
};