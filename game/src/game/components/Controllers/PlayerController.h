#pragma once

#include <object_ptr.hpp>

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

    // We get this using the vehiclecomponent.
    snippetvehicle2::DirectDriveVehicle* vehicle_reference_;

    PlayerState* vehicle_component_;

    // making this a pointer as we want to use it later in the vehicle data structure for changes to speed. 
    Command* executable_command_;

    float timestep_ = 1.f / 60.f;

  public:
    inline void SetGVehicle(snippetvehicle2::DirectDriveVehicle& vehicle)
    {
        vehicle_reference_ = &vehicle;
    }
};
