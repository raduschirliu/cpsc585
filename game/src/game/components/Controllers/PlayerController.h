#pragma once

#include <object_ptr.hpp>

#include "engine/physics/VehicleCommands.h"
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/Transform.h"
#include "game/components/VehicleComponent.h"
#include "game/services/GameStateService.h"

class PlayerState;

class PlayerController final : public Component,
                               public IEventSubscriber<OnUpdateEvent>
{
  public:
    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    void OnUpdate(const Timestep& delta_time) override;
    void OnDebugGui() override;
    std::string_view GetName() const override;

  private:
    jss::object_ptr<Transform> transform_;
    jss::object_ptr<InputService> input_service_;
    jss::object_ptr<GameStateService> game_state_service_;

    jss::object_ptr<PlayerState> player_data_;
    jss::object_ptr<VehicleComponent> vehicle_;

    bool execute_powerup_ = false;
    bool forward_gear_ = true;
    float speed_multiplier_ = 1.0f;
    float handling_multiplier_ = 1.0f;
    VehicleCommand command_;

    void UpdatePowerupControls(const Timestep& delta_time);
    void UpdateCarControls(const Timestep& delta_time);
    void UpdateGear();
    float GetSteerDirection();
    float GetThrottle();
    float GetFrontBrake();
    bool GetGearChangeButton();
};
