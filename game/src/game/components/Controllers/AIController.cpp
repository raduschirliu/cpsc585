#include "AIController.h"

#include <algorithm>
#include <limits>

#include "engine/core/debug/Log.h"
#include "engine/core/math/Physx.h"
#include "engine/input/InputService.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"

float kSpeedMultiplierReset(1.0f);
static constexpr size_t kGamepadId = GLFW_JOYSTICK_1;
float kHandlingMultiplierReset(1.0f);

AIController::AIController()
    : input_service_(nullptr),
      transform_(nullptr),
      ai_service_(nullptr),
      next_path_index_(79)
{
}

void AIController::OnInit(const ServiceProvider& service_provider)
{
    input_service_ = &service_provider.GetService<InputService>();
    ai_service_ = &service_provider.GetService<AIService>();
    transform_ = &GetEntity().GetComponent<Transform>();
    render_service_ = &service_provider.GetService<RenderService>();
    game_state_service_ = &service_provider.GetService<GameStateService>();
    vehicle_ = &GetEntity().GetComponent<VehicleComponent>();
    shooter_ = &GetEntity().GetComponent<Shooter>();

    GetEventBus().Subscribe<OnUpdateEvent>(this);

    if (GetEntity().HasComponent<PlayerState>())
    {
        player_state_ = &GetEntity().GetComponent<PlayerState>();
    }

    path_to_follow_ = ai_service_->GetPath();
    path_traced_.insert(next_path_index_);
}

void AIController::UpdatePowerup()
{
    uint32_t id = game_state_service_->GetEveryoneSlowerSpeedMultiplier();
    if (id != -1)
    {
        if (id != GetEntity().GetId())
        {
            speed_multiplier_ = 0.2f;
            vehicle_->SetMaxAchievableVelocity(40.f);
        }
        else
        {
            // this is the entity which started the powerup, so do nothing.
        }
    }
    else
    {
        // this is the entity which started the powerup, so do nothing.
        speed_multiplier_ = kSpeedMultiplierReset;
        vehicle_->SetMaxAchievableVelocity(100.f);
    }

    id = game_state_service_->GetDisableHandlingMultiplier();

    if (id != -1)
    {
        // now except for the entity who launched it, all the entities should
        // slow down.
        if (GetEntity().GetId() != id)
        {
            // if any AI picked up the powerup then the player's speed should be
            // reduced.
            handling_multiplier_ = 0.0f;
        }
        else
        {
            // this is the entity which started the powerup, so do nothing.
        }
    }
    else
    {
        handling_multiplier_ = kHandlingMultiplierReset;
    }
}

void AIController::OnUpdate(const Timestep& delta_time)
{
    if (game_state_service_->GetRaceState()
            .countdown_elapsed_time.GetSeconds() <=
        game_state_service_->GetMaxCountdownSeconds())
        return;

    glm::vec3 current_car_position = transform_->GetPosition();
    glm::vec3 next_waypoint = path_to_follow_[next_path_index_];

    UpdatePowerup();
    UpdateCarControls(current_car_position, next_waypoint, delta_time);
    NextWaypoint(current_car_position, next_waypoint);
    PowerupDecision();
    CheckShoot(delta_time);
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
    game_state_service_->AddPlayerPowerup(GetEntity().GetId(),
                                          player_state_->GetCurrentPowerup());
}

void AIController::DrawDebugLine(glm::vec3 from, glm::vec3 to)
{
    render_service_->GetDebugDrawList().AddLine(LineVertex(from),
                                                LineVertex(to));
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
        if (speed > 30)
            temp_command.throttle =
                1.0f * (vehicle_->GetAdjustedSpeedMultiplier() / 100) *
                speed_multiplier_;
        else
            temp_command.throttle = 1.0f;
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
        temp_command.steer = 1.0f * handling_multiplier_ * -(projected);
    }
    else
    {
        // std::cout << projected << std::endl;
        temp_command.steer = -1.0f * handling_multiplier_ * projected;
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