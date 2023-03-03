#pragma once

#include <object_ptr.hpp>

#include "engine/game_state/GameStateService.h"
#include "engine/physics/VehicleCommands.h"  // to get the command struct
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/Transform.h"
#include "game/components/VehicleComponent.h"

class PlayerState;

class PlayerController final : public Component,
                               public IEventSubscriber<OnUpdateEvent>
{
  public:
    PlayerController();
    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    void OnUpdate(const Timestep& delta_time) override;
    std::string_view GetName() const override;

  private:
    jss::object_ptr<Transform> transform_;
    jss::object_ptr<InputService> input_service_;
    jss::object_ptr<GameStateService> game_state_;
    // We get this using the vehiclecomponent.
    snippetvehicle2::DirectDriveVehicle* vehicle_reference_;

    bool execute_powerup_ = false;

    PlayerState* player_data_ = nullptr;

    float speed_multiplier_ = 1.f;

    // making this a pointer as we want to use it later in the vehicle data
    // structure for changes to speed.
    Command* executable_command_;

    float timestep_ = 1.f / 60.f;

    void CarController(const Timestep& delta_time);

    float timer_ = 0.f;
    void CheckTimer(double timer_limit, PowerupPickupType pickup_type);

  public:
    inline void SetGVehicle(snippetvehicle2::DirectDriveVehicle& vehicle)
    {
        vehicle_reference_ = &vehicle;
    }
};
