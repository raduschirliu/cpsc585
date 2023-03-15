#include "AIController.h"

#include <algorithm>

#include "engine/core/debug/Log.h"
#include "engine/core/math/Physx.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"

float kSpeedMultiplierReset(1.f);
float kHandlingMultiplierReset(1.f);

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

    GetEventBus().Subscribe<OnUpdateEvent>(this);

    path_to_follow_ = ai_service_->GetPath();
    path_traced_.insert(next_path_index_);
}

void AIController::UpdatePowerup()
{
    if (uint32_t id =
            game_state_service_->GetEveryoneSlowerSpeedMultiplier() != NULL)
    {
        if (id != GetEntity().GetId())
        {
            speed_multiplier_ = 0.2f;
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
    }

    if (uint32_t id =
            game_state_service_->GetDisableHandlingMultiplier() != NULL)
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
    glm::vec3 current_car_position = transform_->GetPosition();
    glm::vec3 next_waypoint = path_to_follow_[next_path_index_];

    UpdateCarControls(current_car_position, next_waypoint, delta_time);
    NextWaypoint(current_car_position, next_waypoint);
}

void AIController::DrawDebugLine(glm::vec3 from, glm::vec3 to)
{
    render_service_->GetDebugDrawList().AddLine(
        LineVertex(transform_->GetPosition()),
        LineVertex(glm::vec3(path_to_follow_[next_path_index_].x,
                             path_to_follow_[next_path_index_].y + 10,
                             path_to_follow_[next_path_index_].z)));
}

void AIController::UpdateCarControls(glm::vec3& current_car_position,
                                     glm::vec3& next_waypoint,
                                     const Timestep& delta_time)
{
    // ----------------- put the code from here to updatecarcontrols
    glm::vec3 current_car_right_direction = transform_->GetRightDirection();

    VehicleCommand temp_command;

    float speed = vehicle_->GetSpeed();
    if (speed <= 40)
    {
        temp_command.throttle = 1.f * speed_multiplier_;
    }
    else
    {
        temp_command.throttle = 0.f;
    }

    glm::vec3 waypoint_dir = normalize(next_waypoint - current_car_position);
    float projected = dot(waypoint_dir, current_car_right_direction);

    DrawDebugLine(
        current_car_position,
        glm::vec3(next_waypoint.x, next_waypoint.y + 10, next_waypoint.z));

    if (projected <= 0.1 && projected >= -0.1)
    {
        temp_command.steer = 0.f;
    }
    else if (projected < 0)
    {
        temp_command.steer = 1.f * handling_multiplier_ * -(projected) ;
    }
    else
    {
        //std::cout << projected << std::endl;
        temp_command.steer = -1.f * handling_multiplier_ * projected;
    }

    vehicle_->SetCommand(temp_command);
}

void AIController::NextWaypoint(glm::vec3& current_car_position,
                                glm::vec3 next_waypoint)
{
    float distance = glm::distance(current_car_position, next_waypoint);

    if (distance < 50.f)
    {
        int min_index = 0;
        float min_distance = static_cast<float>(INT_MAX);
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