#pragma once

#include <object_ptr.hpp>
#include <set>

#include "engine/AI/AIService.h"
#include "engine/physics/VehicleCommands.h"  // to get the command struct
#include "engine/render/RenderService.h"
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/Transform.h"
#include "game/components/VehicleComponent.h"
#include "game/components/state/PlayerState.h"
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
    void ResetForNextLap();

  private:
    jss::object_ptr<Transform> transform_;
    jss::object_ptr<InputService> input_service_;
    jss::object_ptr<AIService> ai_service_;
    jss::object_ptr<RenderService> render_service_;
    jss::object_ptr<GameStateService> game_state_service_;

    jss::object_ptr<PlayerState> player_state_;

    // variable which changes when the speed slower powerup is picked up.
    float speed_multiplier_ = 1.f;

    float handling_multiplier_ = 1.f;

    // variables for car

    std::vector<glm::vec3> path_to_follow_;
    jss::object_ptr<VehicleComponent> vehicle_;

    void UpdateCarControls(glm::vec3& current_car_position,
                           glm::vec3& next_waypoint,
                           const Timestep& delta_time);
    void NextWaypoint(glm::vec3& current_car_position, glm::vec3 next_waypoint);
    void DrawDebugLine(glm::vec3 from, glm::vec3 to);
    void UpdatePowerup();
    void ExecutePowerup();
    void PowerupDecision();

    // as we want the car to move from current to next command, and so on until
    // the end.
    int next_path_index_;

    std::set<int> path_traced_;
};
