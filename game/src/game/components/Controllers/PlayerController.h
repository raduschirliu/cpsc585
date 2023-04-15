#pragma once

#include <object_ptr.hpp>

#include "engine/fwd/FwdComponents.h"
#include "engine/fwd/FwdServices.h"
#include "engine/physics/VehicleCommands.h"
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"
#include "game/FwdGame.h"
#include "game/components/shooting/Shooter.h"

class PickupService;
class AudioService;

class PlayerController final : public Component,
                               public IEventSubscriber<OnUpdateEvent>
{
  public:
    /* ----- from component ----- */

    void OnInit(const ServiceProvider& service_provider) override;
    void OnUpdate(const Timestep& delta_time) override;
    void OnDebugGui() override;
    std::string_view GetName() const override;

  private:
    jss::object_ptr<Transform> transform_;
    jss::object_ptr<InputService> input_service_;
    jss::object_ptr<GameStateService> game_state_service_;
    jss::object_ptr<AudioService> audio_service_;

    jss::object_ptr<PlayerState> player_state_;
    jss::object_ptr<VehicleComponent> vehicle_;
    jss::object_ptr<Shooter> shooter_;
    jss::object_ptr<PickupService> pickup_service_;

    bool execute_powerup_ = false;
    bool forward_gear_ = true;
    float speed_multiplier_ = 1.0f;
    float handling_multiplier_ = 1.0f;
    VehicleCommand command_;

    // to respawn the car
    double respawn_timer_ = 0.0f;
    float shoot_cooldown_ = 0.0f;

    void CheckShoot(const Timestep& delta_time);
    void UpdatePowerupControls(const Timestep& delta_time);
    void UpdateCarControls(const Timestep& delta_time);
    void UpdateGear();
    float GetSteerDirection();
    float GetThrottle();
    float GetFrontBrake();
    float GetRearBrake();
    bool GetGearChangeButton();
};
