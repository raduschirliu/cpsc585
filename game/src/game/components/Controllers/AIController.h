#pragma once

#include <object_ptr.hpp>

#include "engine/physics/VehicleCommands.h"  // to get the command struct
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/Transform.h"
#include "engine/AI/AIService.h"
#include "game/components/VehicleComponent.h"

class AIController final : public Component,
                               public IEventSubscriber<OnUpdateEvent>
{
  public:
    AIController();
    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    void OnUpdate(const Timestep& delta_time) override;
    std::string_view GetName() const override;

  private:
    jss::object_ptr<Transform> transform_;
    jss::object_ptr<InputService> input_service_;
    jss::object_ptr<AIService> ai_service_;

    // We get this using the vehiclecomponent.
    DirectDriveVehicle* vehicle_reference_;

    Command executable_command_;

    float timestep_ = 1.f / 60.f;

    std::vector<glm::vec3> path_to_follow_;

  public:
    inline void SetGVehicle(DirectDriveVehicle& vehicle)
    {
        vehicle_reference_ = &vehicle;
    }
};
