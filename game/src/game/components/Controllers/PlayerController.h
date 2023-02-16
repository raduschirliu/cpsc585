#pragma once

#include <object_ptr.hpp>

#include "engine/physics/HelperUtils.h"  // to get the command struct
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/Transform.h"
#include "game/components/VehicleComponent.h"

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
    DirectDriveVehicle* g_vehicle_;

    Command executable_command_;

    float timestep = 1.f / 60.f;

  public:
    inline void SetGVehicle(DirectDriveVehicle& vehicle)
    {
        g_vehicle_ = &vehicle;
    }
};
