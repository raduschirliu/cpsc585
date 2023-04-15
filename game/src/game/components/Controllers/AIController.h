#pragma once

#include <object_ptr.hpp>
#include <set>

#include "engine/fwd/FwdComponents.h"
#include "engine/fwd/FwdServices.h"
#include "engine/physics/VehicleCommands.h"  // to get the command struct
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"
#include "game/FwdGame.h"
#include "game/components/state/PlayerState.h"

class PickupService;
class Shooter;

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

    // RESPAWN

    // starting the timer to carry out the respawning logic
    void SetRespawnLastCheckpointTimer(bool b_value);

    // Returns boolean if the timer is on or not.
    bool GetRespawnLastCheckpointTimer();

  private:
    jss::object_ptr<Transform> transform_;
    jss::object_ptr<InputService> input_service_;
    jss::object_ptr<AIService> ai_service_;
    jss::object_ptr<RenderService> render_service_;
    jss::object_ptr<GameStateService> game_state_service_;
    jss::object_ptr<PhysicsService> physics_service_;
    jss::object_ptr<PickupService> pickup_service_;

    jss::object_ptr<Shooter> shooter_;
    jss::object_ptr<PlayerState> player_state_;

    // variable which changes when the speed slower powerup is picked up.
    float speed_multiplier_ = 1.f;
    float shoot_cooldown_;
    float handling_multiplier_ = 1.f;

    // if the car just respawned then it should wait for some time to let it
    // respawn again, doing this just because of the minimum threshold respawn
    // ability
    bool respawn_tracker_ = false;
    double minimum_threshold_respawn_timer_ = 0.f;
    // as we want the car to move from current to next command, and so on
    // until the end.
    int next_path_index_;

    std::set<int> path_traced_;

    // variables for car

    std::vector<glm::vec3> path_to_follow_;
    jss::object_ptr<VehicleComponent> vehicle_;

    void UpdateCarControls(const glm::vec3& current_car_position,
                           const glm::vec3& next_waypoint,
                           const Timestep& delta_time);
    void NextWaypoint(const glm::vec3& current_car_position,
                      const glm::vec3& next_waypoint);
    void DrawDebugLine(const glm::vec3 from, const glm::vec3 to);
    void UpdatePowerup();
    void ExecutePowerup();
    void PowerupDecision();

    /// @brief will randomly determine whether the AI will shoot or not.
    ///   the higher the chance, the more likely it shoots.
    bool WillShoot(float chance = 0.5f);
    void CheckShoot(const Timestep& delta_time);

    // respawn handling

    // this will be set in the onstart of the controller, so that the we can
    // store the initial height of the car in the world and then later compare
    // if the height difference is more than 500 then we can respawn the car.
    float initial_height_ = 0.f;

    double respawn_timer_missed_checkpoint_ = 0.0f;
    bool b_respawn_timer_missed_checkpoint_ = false;

    double respawn_timer_min_speed = 0.0f;
    void HandleRespawn(const Timestep& delta_time);
    void HandleFreefallRespawn(const Timestep& delta_time);
    void HandleMissedCheckpointRespawn(const Timestep& delta_time);
    void FixRespawnOrientation(const glm::vec3& next_checkpoint_location,
                               const glm::vec3& checkpoint_location);
};
