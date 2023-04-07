#include "AIController.h"

#include <algorithm>
#include <limits>
#include <random>

#include "engine/core/debug/Log.h"
#include "engine/core/math/Physx.h"
#include "engine/input/InputService.h"
#include "engine/physics/PhysicsService.h"
#include "engine/pickup/PickupService.h"
#include "engine/scene/Entity.h"

float kSpeedMultiplier(0.1f);
static constexpr size_t kGamepadId = GLFW_JOYSTICK_1;
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

    glm::vec3 current_car_position = transform_->GetPosition();
    glm::vec3 next_waypoint = path_to_follow_[next_path_index_];

    UpdatePowerup();
    UpdateCarControls(current_car_position, next_waypoint, delta_time);
    NextWaypoint(current_car_position, next_waypoint);

    if (!respawn_tracker_)
    {
        HandleRespawn(delta_time);
    }
    PowerupDecision();

    // note: raycasting *may* be an expensive approach
    glm::vec3 forward = transform_->GetForwardDirection();
    glm::vec3 offset = 20.0f * forward;
    glm::vec3 origin = transform_->GetPosition() + offset;

    // if an entity is within view
    bool saw_something =
        physics_service_->RaycastDynamic(origin, forward, kViewDistance)
            .has_value();
    if (saw_something && WillShoot(0.75f))
    {
        CheckShoot(delta_time);
    }
}

void AIController::FixRespawnOrientation(const glm::vec3& next_checkpoint,
                                         const glm::vec3& last_checkpoint)
{
    auto current_orientation = transform_->GetOrientation();

    // assume car is initially oriented along the negative z-axis
    glm::vec3 forward = transform_->GetForwardDirection();
    glm::vec3 direction = glm::normalize(next_checkpoint - last_checkpoint);
    glm::vec3 axis = glm::normalize(glm::cross(forward, direction));
    float angle = glm::acos(glm::dot(forward, direction));

    transform_->SetOrientation(glm::angleAxis(angle, axis) *
                               current_orientation);
}

void AIController::HandleRespawn(const Timestep& delta_time)
{
    // if the velocity of car is less than some amount then respawn the car.
    HandleMinSpeedThresholdRespawn(delta_time);

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
        glm::vec3 checkpoint_location;
        glm::vec3 next_checkpoint_location;

        int current_checkpoint = game_state_service_->GetCurrentCheckpoint(
            this->GetEntity().GetId(), checkpoint_location,
            next_checkpoint_location);

        if (current_checkpoint == -1)
            return;

        vehicle_->Respawn();

        // transform_->SetPosition(checkpoint_location);
        // FixRespawnOrientation(next_checkpoint_location, checkpoint_location);
        // // add this car's id to respawn, which will be handled by the
        // // gamestateservice
        // game_state_service_->AddRespawnPlayers(this->GetEntity().GetId());
    }
}

void AIController::HandleMinSpeedThresholdRespawn(const Timestep& delta_time)
{
    if (vehicle_->GetSpeed() < kMinRespawnSpeed)
    {
        respawn_timer_min_speed += delta_time.GetSeconds();
        if (respawn_timer_min_speed >= kMaxRespawnTimer)
        {
            // reset the transform to that of the checkpoint
            glm::vec3 checkpoint_location;
            glm::vec3 next_checkpoint_location;

            int current_checkpoint = game_state_service_->GetCurrentCheckpoint(
                this->GetEntity().GetId(), checkpoint_location,
                next_checkpoint_location);

            if (current_checkpoint == -1)
                return;

            vehicle_->Respawn();

            // transform_->SetPosition(checkpoint_location);
            // FixRespawnOrientation(next_checkpoint_location,
            //                       checkpoint_location);
            // // add this car's id to respawn, which will be handled by the
            // // gamestateservice
            // game_state_service_->AddRespawnPlayers(this->GetEntity().GetId());

            // as now we do not want the AI to get in an infinite respawn loop.
            respawn_tracker_ = true;
        }
    }
    else
    {
        // reset the timer back
        respawn_timer_min_speed = 0.0f;
    }
}

void AIController::HandleFreefallRespawn(const Timestep& delta_time)
{
    if (glm::abs(transform_->GetPosition().y - initial_height_) >
        kMaxFreeFallDifference)
    {
        // reset the transform to that of the checkpoint
        // TODO: Set the orientation based on the next and current checkpoint.
        glm::vec3 checkpoint_location;
        glm::vec3 next_checkpoint_location;

        int current_checkpoint = game_state_service_->GetCurrentCheckpoint(
            this->GetEntity().GetId(), checkpoint_location,
            next_checkpoint_location);

        if (current_checkpoint == -1)
            return;

        vehicle_->Respawn();

        // transform_->SetPosition(checkpoint_location);
        // FixRespawnOrientation(next_checkpoint_location, checkpoint_location);
        // // as this indicates that the car has fallen off the map, respawn it
        // to
        // // the previous checkpoint
        // game_state_service_->AddRespawnPlayers(this->GetEntity().GetId());
    }
}

bool AIController::WillShoot(float chance)
{
    std::random_device seed;
    std::mt19937 generator(seed());
    std::uniform_int_distribution<int> value(0, 100);

    return value(generator) < chance * 100;
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
    // srand(time(0));
    // as this is happening every loop, we need to make sure that the
    // probability to execute the powerup is really low
    int probability_powerup_execution = (rand() % 100);

    // debug::LogDebug("{}, {} with random number:", GetEntity().GetName(),
    // probability_powerup_execution);

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

void AIController::DrawDebugLine(glm::vec3 from, glm::vec3 to)
{
    render_service_->GetDebugDrawList().AddLine(DebugVertex(from),
                                                DebugVertex(to));
}

void AIController::UpdateCarControls(glm::vec3& current_car_position,
                                     glm::vec3& next_waypoint,
                                     const Timestep& delta_time)
{
    // ----------------- put the code from here to updatecarcontrols
    glm::vec3 current_car_right_direction = -transform_->GetRightDirection();

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

    glm::vec3 waypoint_dir = normalize(next_waypoint - current_car_position);
    float projected = dot(waypoint_dir, current_car_right_direction);

    // DrawDebugLine(
    //     current_car_position,
    //     glm::vec3(next_waypoint.x, next_waypoint.y + 10, next_waypoint.z));

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

void AIController::NextWaypoint(glm::vec3& current_car_position,
                                glm::vec3 next_waypoint)
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
