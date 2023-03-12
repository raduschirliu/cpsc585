#include "AIController.h"

#include <algorithm>

#include "engine/core/debug/Log.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"

float kSpeedMultiplierReset(1.f);
float kHandlingMultiplierReset(1.f);

AIController::AIController()
    : input_service_(nullptr),
      transform_(nullptr),
      ai_service_(nullptr)
{
}

void AIController::OnInit(const ServiceProvider& service_provider)
{
    input_service_ = &service_provider.GetService<InputService>();
    ai_service_ = &service_provider.GetService<AIService>();
    transform_ = &GetEntity().GetComponent<Transform>();
    game_state_service_ = &service_provider.GetService<GameStateService>();
    GetEventBus().Subscribe<OnUpdateEvent>(this);

    path_to_follow_ = ai_service_->GetPath();
    next_car_position_ = path_to_follow_[34];
}

float AIController::PIDController(float targetPosition, float currentPosition,
                                  float deltaTime)
{
    // Set the PID constants
    float Kp = 1.0f;
    float Ki = 0.0f;
    float Kd = 0.0f;

    // Set the error variables
    float error = targetPosition - currentPosition;
    static float accumulatedError = 0.0f;
    static float previousError = 0.0f;

    // Calculate the PID terms
    float pTerm = Kp * error;
    accumulatedError += error * deltaTime;
    float iTerm = Ki * accumulatedError;
    float dTerm = Kd * ((error - previousError) / deltaTime);

    // Calculate the total output
    float output = pTerm + iTerm + dTerm;

    // Update the previous error
    previousError = error;

    // Clamp the output to a valid range (-1 to 1 for DirectDriveVehicle)
    output = std::min(output, 1.0f);
    output = std::max(output, -1.0f);

    // Return the output as the steering angle
    return output;
}

// Constants for the PID controller
const float kP = 0.1f;
const float kI = 0.01f;
const float kD = 0.05f;
const float maxSteeringAngle = 1.0f;

// Initialize PID controller variables
float integralError = 0.0f;
float previousError = 0.0f;

void AIController::OnUpdate(const Timestep& delta_time)
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

    /**
     * Here we have different criteria on how we want the car to move, the more
     * advanced this is, the better the car will move on the track
     * **/
    float speed = vehicle_reference_->mPhysXState.physxActor.rigidBody
                      ->getLinearVelocity()
                      .magnitude();
    // Log::debug("{}", speed);
    if (speed <= 40)
    {
        vehicle_reference_->mCommandState.throttle =
            1.f * speed_multiplier_;  // for the everyone slow down pickup.
    }
    else
    {
        vehicle_reference_->mCommandState.throttle =
            0.f;  // for the everyone slow down pickup.
    }

    // glm::vec3 toTarget =
    //     glm::normalize(next_car_position_ - transform_->GetPosition());

    // // Log::debug("front {}, {}, {}", transform_->GetForwardDirection().x,
    // transform_->GetForwardDirection().y,
    // //            transform_->GetForwardDirection().z);

    // float dot = glm::dot(transform_->GetForwardDirection(), toTarget);

    // if (sqrt(dot * dot) > 0.99f)
    //     vehicle_reference_->mCommandState.steer = 0.f;
    // else
    // {
    //     glm::vec3 cross =
    //         glm::cross(transform_->GetForwardDirection(), toTarget);
    //     cross = glm::normalize(cross);
    //     //Log::debug("{}", cross.x);
    //     if (cross.x > 0)
    //     {
    //         Log::debug("Steer left");
    //         vehicle_reference_->mCommandState.steer = -0.6f;
    //     }
    //     else
    //     {
    //         Log::debug("Steer right");
    //         vehicle_reference_->mCommandState.steer = 1.f;
    //     }
    // }

    glm::vec3 waypoint_dir =
        normalize(next_car_position_ - transform_->GetPosition());
    auto projected = dot(waypoint_dir, transform_->GetRightDirection());

    if (projected == 0)
    {
        vehicle_reference_->mCommandState.steer = 0.f;
    }
    else if (projected < 0)
    {
        // Log::debug("Turn right");
        vehicle_reference_->mCommandState.steer = -1.f * projected;
    }
    else
    {
        // Log::debug("Turn left");
        vehicle_reference_->mCommandState.steer = 1.f * projected;
    }

    // calculate the euclidean distance to see if the car is near the next
    // position, if yes then update the next position to be the next index in
    // the path array
    float distance = glm::distance(transform_->GetPosition(),
                                   path_to_follow_[next_path_index_]);
    Log::debug("Distance to the next point {}", distance);
    if (distance < 100.f)
    {
        Log::debug("previous path was: {}, {}, {}",
                   path_to_follow_[next_path_index_].x,
                   path_to_follow_[next_path_index_].y,
                   path_to_follow_[next_path_index_].z);
        // next_car_position_ = path_to_follow_[next_path_index_--];
        int min_index = 0.f;
        float min_distance = INT_MAX;
        // find the smallest path which not has been traversed yet.
        for (int i = 0; i < path_to_follow_.size(); i++)
        {
            // if path is not traced yet.
            if (path_traced_.find(i) == path_traced_.end())
            {
                float dist = glm::distance(path_to_follow_[i],
                                           path_to_follow_[next_path_index_]);
                if (min_distance > dist)
                {
                    min_index = i;
                    min_distance = dist;
                }
            }
        }
        next_path_index_ = min_index;
        path_traced_.insert(next_path_index_);
        // next_car_position_ = path_to_follow_[next_path_index_];
        // next_path_index_ = 31;
        Log::debug("Next path to follow: {}, {}, {}",
                   path_to_follow_[next_path_index_].x,
                   path_to_follow_[next_path_index_].y,
                   path_to_follow_[next_path_index_].z);
    }
    //     // else
    //     // {
    //     //     if (path_traced_.find(next_path_index_ + 1) !=
    //     path_traced_.end())
    //     //     {
    //     //         int min_index = 0.f;
    //     //         float min_distance = INT_MAX;
    //     //         // find the smallest path which not has been traversed
    //     yet.
    //     //         for (int i = 0; i < path_to_follow_.size(); i++)
    //     //         {
    //     //             // if path is not traced yet.
    //     //             if (path_traced_.find(i) == path_traced_.end())
    //     //             {
    //     //                 float dist =
    //     //                     glm::distance(path_to_follow_[i],
    //     // path_to_follow_[next_path_index_]);
    //     //                 if (min_distance > dist)
    //     //                 {
    //     //                     min_index = i;
    //     //                     min_distance = dist;
    //     //                 }
    //     //             }
    //     //         }
    //     //         next_path_index_ = min_index;
    //     //     }
    //     //     else
    //     //     {
    //     //         next_car_position_ = path_to_follow_[next_path_index_ +=
    //     1];
    //     //     }
    //     // }
    //     // path_traced_.insert(next_path_index_);

    //     // if (next_path_index_ == 706)
    //     // {
    //     //     int min_index = 0.f;
    //     //     float min_distance = INT_MAX;
    //     //     // find the smallest path which not has been traversed yet.
    //     //     for (int i = 0; i < path_to_follow_.size(); i++)
    //     //     {
    //     //         // if path is not traced yet.
    //     //         if (path_traced_.find(i) == path_traced_.end())
    //     //         {
    //     //             float dist = glm::distance(
    //     //                 path_to_follow_[i],
    //     path_to_follow_[next_path_index_]);
    //     //             if (min_distance > dist)
    //     //             {
    //     //                 min_index = i;
    //     //                 min_distance = dist;
    //     //             }
    //     //         }
    //     //     }
    //     //     next_path_index_ = min_index;
    //     // }

    // Log::debug("{}", next_path_index_);
}

std::string_view AIController::GetName() const
{
    return "AI Controller";
}

void AIController::SetGVehicle(snippetvehicle2::DirectDriveVehicle& vehicle)
{
    vehicle_reference_ = &vehicle;
}
