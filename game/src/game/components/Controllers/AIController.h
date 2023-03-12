#pragma once

#include <object_ptr.hpp>

#include "engine/AI/AIService.h"
#include "engine/physics/VehicleCommands.h"  // to get the command struct
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/Transform.h"
#include "game/components/VehicleComponent.h"
#include "game/services/GameStateService.h"

class AIController final : public Component,
                           public IEventSubscriber<OnUpdateEvent>
{
  public:
    AIController();
    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    void OnUpdate(const Timestep& delta_time) override;
    std::string_view GetName() const override;
    void SetGVehicle(snippetvehicle2::DirectDriveVehicle& vehicle);

  private:
    jss::object_ptr<Transform> transform_;
    jss::object_ptr<InputService> input_service_;
    jss::object_ptr<AIService> ai_service_;
    jss::object_ptr<GameStateService> game_state_service_;

    // variable which changes when the speed slower powerup is picked up.
    float speed_multiplier_ = 1.f;

    float handling_multiplier_ = 1.f;

    // variables for car

    std::vector<glm::vec3> path_to_follow_;
    snippetvehicle2::DirectDriveVehicle* vehicle_reference_;

    VehicleCommand executable_command_;
    float timestep_ = 1.f / 60.f;

    // as we want the car to move from current to next command, and so on until
    // the end.
    glm::vec3 next_car_position_;
    int next_path_index_ = 358;
};
