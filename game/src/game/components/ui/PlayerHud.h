#pragma once

#include <object_ptr.hpp>

#include "engine/gui/OnGuiEvent.h"
#include "engine/scene/Component.h"

class InputService;
class VehicleComponent;
class GameStateService;
class PlayerState;

class PlayerHud final : public Component, public IEventSubscriber<OnGuiEvent>
{
  public:
    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    std::string_view GetName() const override;

    // From IEventSubscriber<OnGuiEvent>
    void OnGui() override;

  private:
    jss::object_ptr<GameStateService> game_state_service_;

    jss::object_ptr<VehicleComponent> vehicle_;
    jss::object_ptr<PlayerState> player_state_;
};
