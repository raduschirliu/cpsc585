#include "AIController.h"

#include <algorithm>
#include <limits>

#include "engine/AI/AIService.h"
#include "engine/core/debug/Log.h"
#include "engine/core/math/Physx.h"
#include "engine/core/math/Random.h"
#include "engine/input/InputService.h"
#include "engine/physics/PhysicsService.h"
#include "engine/pickup/PickupService.h"
#include "engine/render/RenderService.h"
#include "engine/scene/Entity.h"
#include "engine/scene/Transform.h"
#include "game/components/VehicleComponent.h"
#include "game/components/shooting/Shooter.h"
#include "game/services/GameStateService.h"

using glm::vec3;

float kSpeedMultiplier(0.1f);
float kHandlingMultiplier(0.0f);
static constexpr float kMaxRespawnTimer(3.0f);
static constexpr float kMaxFreeFallDifference(500.0f);

// if the AI is below this speed then the car will respawn to the
// last checkpoint as there is some problem with the AI.
static constexpr float kMinRespawnSpeed(11.0f);
static constexpr double kMaxCheckpointMissedTimer(5.f);

// distance that AI can "see"; shoots when a target is within view
static constexpr float kViewDistance(300.f);

AIController::AIController()
    : input_service_(nullptr),
      transform_(nullptr),
      ai_service_(nullptr),
      next_path_index_(81)
{
}

void AIController::OnInit(const ServiceProvider& service_provider)
{
    // service dependencies
    input_service_ = &service_provider.GetService<InputService>();
    ai_service_ = &service_provider.GetService<AIService>();
    transform_ = &GetEntity().GetComponent<Transform>();
    render_service_ = &service_provider.GetService<RenderService>();
    game_state_service_ = &service_provider.GetService<GameStateService>();
    physics_service_ = &service_provider.GetService<PhysicsService>();
    pickup_service_ = &service_provider.GetService<PickupService>();

    // component dependencies
    vehicle_ = &GetEntity().GetComponent<VehicleComponent>();
    shooter_ = &GetEntity().GetComponent<Shooter>();

    GetEventBus().Subscribe<OnUpdateEvent>(this);

    if (GetEntity().HasComponent<PlayerState>())
    {
        player_state_ = &GetEntity().GetComponent<PlayerState>();
    }

    path_to_follow_ = ai_service_->GetPath();
    path_traced_.insert(next_path_index_);

    // for respawning.
    initial_height_ = transform_->GetPosition().y;
}

// Execute the powerup.
void AIController::UpdatePowerup()
{
    switch (player_state_->GetCurrentPowerup())
    {
        case PowerupPickupType::kDisableHandling:
            // handle executing the powerup
            pickup_service_->AddEntityWithPowerup(&GetEntity(),
                                                  "DisableHandling");
            pickup_service_->AddEntityWithTimer(&GetEntity(), 0.0f);
            break;

        case PowerupPickupType::kEveryoneSlower:
            // handle executing the powerup
            pickup_service_->AddEntityWithPowerup(&GetEntity(),
                                                  "EveryoneSlower");
            pickup_service_->AddEntityWithTimer(&GetEntity(), 0.0f);
            break;

        case PowerupPickupType::kIncreaseAimBox:
            // handle executing the powerup
            pickup_service_->AddEntityWithPowerup(&GetEntity(),
                                                  "IncreaseAimBox");
            pickup_service_->AddEntityWithTimer(&GetEntity(), 0.0f);
            break;

        case PowerupPickupType::kKillAbilities:
            // handle executing the powerup
            pickup_service_->AddEntityWithPowerup(&GetEntity(),
                                                  "KillAbilities");
            pickup_service_->AddEntityWithTimer(&GetEntity(), 0.0f);
            break;
    }
}

void AIController::OnUpdate(const Timestep& delta_time)
{
    if (respawn_tracker_)
    {
        minimum_threshold_respawn_timer_ += delta_time.GetSeconds();
        if (minimum_threshold_respawn_timer_ >= kMaxRespawnTimer * 4)
        {
            respawn_tracker_ = false;
            minimum_threshold_respawn_timer_ = 0.f;
            respawn_timer_min_speed = 0.f;
        }
    }

    if (game_state_service_->GetRaceState()
            .countdown_elapsed_time.GetSeconds() <=
        game_state_service_->GetMaxCountdownSeconds())
    {
        return;
    }

    // do nothing when dead
    if (player_state_->IsDead())
    {
        return;
    }

    const vec3& current_car_position = transform_->GetPosition();
    const vec3& next_waypoint = path_to_follow_[next_path_index_];

    UpdatePowerup();
    UpdateCarControls(current_car_position, next_waypoint, delta_time);
    NextWaypoint(current_car_position, next_waypoint);

    // Disabled due to AIs respawning continuously at the ramp and causing huge
    // frame drops if (!respawn_tracker_)
    // {
    //     HandleRespawn(delta_time);
    // }
    PowerupDecision();

    // note: raycasting *may* be an expensive approach
    const vec3& forward = transform_->GetForwardDirection();
    const vec3 offset = 20.0f * forward;
    const vec3 origin = transform_->GetPosition() + offset;

    // if an entity is within view
    const bool saw_something =
        physics_service_->RaycastDynamic(origin, forward, kViewDistance)
            .has_value();
    if (saw_something && WillShoot(0.75f))
    {
        CheckShoot(delta_time);
    }
}

void AIController::FixRespawnOrientation(const vec3& next_checkpoint,
                                         const vec3& last_checkpoint)
{
    const auto& current_orientation = transform_->GetOrientation();

    // assume car is initially oriented along the negative z-axis
    vec3 forward = transform_->GetForwardDirection();
    vec3 direction = glm::normalize(next_checkpoint - last_checkpoint);
    vec3 axis = glm::normalize(glm::cross(forward, direction));
    float angle = glm::acos(glm::dot(forward, direction));

    transform_->SetOrientation(glm::angleAxis(angle, axis) *
                               current_orientation);
}

void AIController::HandleRespawn(const Timestep& delta_time)
{
    // if the car falls off the map
    HandleFreefallRespawn(delta_time);

    // if the AI is following the wrong path for more than 6 seconds, missed a
    // checkpoint (reset it to the last checkpoint it crossed).
    HandleMissedCheckpointRespawn(delta_time);
}

void AIController::HandleMissedCheckpointRespawn(const Timestep& delta_time)
{
    // the respawn_timer_missed_checkpoint_ timer is started by
    // gamestateservice, when it detects that it is enemy car who is not
    // following the correct checkpoint sequence, to reset it and respawn it at
    // correct location

    // set by the gamestateservice.
    if (b_respawn_timer_missed_checkpoint_)
    {
        respawn_timer_missed_checkpoint_ += delta_time.GetSeconds();
    }

    if (respawn_timer_missed_checkpoint_ >= kMaxCheckpointMissedTimer)
    {
        // as the car should respawn to the previous checkpoint now,
        // applying that logic
        vec3 checkpoint_location;
        vec3 next_checkpoint_location;

        int current_checkpoint = game_state_service_->GetCurrentCheckpoint(
            this->GetEntity().GetId(), checkpoint_location,
            next_checkpoint_location);

        if (current_checkpoint == -1)
        {
            return;
        }

        vehicle_->Respawn();
    }
}

void AIController::HandleFreefallRespawn(const Timestep& delta_time)
{
    if (glm::abs(transform_->GetPosition().y - initial_height_) >
        kMaxFreeFallDifference)
    {
        // reset the transform to that of the checkpoint
        // TODO: Set the orientation based on the next and current checkpoint.
        vec3 checkpoint_location;
        vec3 next_checkpoint_location;

        int current_checkpoint = game_state_service_->GetCurrentCheckpoint(
            this->GetEntity().GetId(), checkpoint_location,
            next_checkpoint_location);

        if (current_checkpoint == -1)
            return;

        vehicle_->Respawn();
    }
}

bool AIController::WillShoot(float chance)
{
    return math::RandomInt(0, 100) < chance * 100;
}

void AIController::CheckShoot(const Timestep& delta_time)
{
    if (shoot_cooldown_ > 0.0f)
    {
        shoot_cooldown_ -= static_cast<float>(delta_time.GetSeconds());
        return;
    }

    shooter_->Shoot();
    shoot_cooldown_ = shooter_->GetCooldownTime();
}

// Decision for Powerup.
void AIController::PowerupDecision()
{
    // as this is happening every loop, we need to make sure that the
    // probability to execute the powerup is really low
    int probability_powerup_execution = (rand() % 100);

    if (probability_powerup_execution == 99)
    {
        ExecutePowerup();
    }
}

// Execute the powerup.
void AIController::ExecutePowerup()
{
    // TODO: handle executing the powerup
}

void AIController::DrawDebugLine(vec3 from, vec3 to)
{
    render_service_->GetDebugDrawList().AddLine(DebugVertex(from),
                                                DebugVertex(to));
}

void AIController::UpdateCarControls(const vec3& current_car_position,
                                     const vec3& next_waypoint,
                                     const Timestep& delta_time)
{
    // ----------------- put the code from here to updatecarcontrols
    vec3 current_car_right_direction = -transform_->GetRightDirection();

    VehicleCommand temp_command;

    float speed = vehicle_->GetSpeed();
    if (speed <= 99)
    {
        // debug::LogWarn("{}", speed_multiplier_);
        if (speed > 55)
        {
            temp_command.throttle =
                1.0f * (vehicle_->GetAdjustedSpeedMultiplier() / 100) *
                speed_multiplier_;
        }
        else
        {
            if (pickup_service_->IsVehicleSlowDown(&GetEntity()))
                temp_command.throttle = 1.0f * kSpeedMultiplier;
            else
                temp_command.throttle = 1.0f;
        }
    }
    else
    {
        temp_command.throttle = 0.0f;
    }

    if (speed > 99)
    {
        temp_command.front_brake = 0.5f;
        temp_command.rear_brake = 0.5f;
    }

    vec3 waypoint_dir = normalize(next_waypoint - current_car_position);
    float projected = dot(waypoint_dir, current_car_right_direction);

    // DrawDebugLine(
    //     current_car_position,
    //     vec3(next_waypoint.x, next_waypoint.y + 10, next_waypoint.z));

    if (projected <= 0.1f && projected >= -0.1f)
    {
        temp_command.steer = 0.0f;
    }
    else if (projected < 0)
    {
        if (pickup_service_->IsVehicleDisableHandling(&GetEntity()))
        {
            temp_command.steer = 1.0f * -(projected)*kHandlingMultiplier;
        }
        else
        {
            temp_command.steer = 1.0f;
        }
    }
    else
    {
        if (pickup_service_->IsVehicleDisableHandling(&GetEntity()))
        {
            temp_command.steer = -1.0f * -(projected)*kHandlingMultiplier;
        }
        else
        {
            temp_command.steer = -1.0f;
        }
    }

    vehicle_->SetCommand(temp_command);
}

void AIController::NextWaypoint(const vec3& current_car_position,
                                const vec3& next_waypoint)
{
    float distance = glm::distance(current_car_position, next_waypoint);

    if (distance < 50.0f)
    {
        int min_index = 0;
        float min_distance = std::numeric_limits<float>::max();
        // find the smallest path which not has been traversed yet.
        for (int i = 0; i < path_to_follow_.size(); i++)
        {
            // if path is not traced yet.
            if (path_traced_.find(i) == path_traced_.end())
            {
                float dist = glm::distance(path_to_follow_[i], next_waypoint);
                if (min_distance > dist)
                {
                    min_index = i;
                    min_distance = dist;
                }
            }
        }
        next_path_index_ = min_index;
        path_traced_.insert(next_path_index_);
    }
}

std::string_view AIController::GetName() const
{
    return "AI Controller";
}

void AIController::ResetForNextLap()
{
    // clearing the traced path for the next path now
    path_traced_.clear();
    next_path_index_ = 79;
    path_traced_.insert(next_path_index_);
}

// starting the timer to carry out the respawning logic
void AIController::SetRespawnLastCheckpointTimer(bool b_value)
{
    b_respawn_timer_missed_checkpoint_ = b_value;

    // reset the timer back to 0
    if (!b_value)
    {
        respawn_timer_missed_checkpoint_ = 0.0f;
    }
}

// Returns boolean if the timer is on or not.
bool AIController::GetRespawnLastCheckpointTimer()
{
    return b_respawn_timer_missed_checkpoint_;
}
