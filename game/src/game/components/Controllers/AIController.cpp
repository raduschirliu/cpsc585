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

    // Log::debug("Previous Position: {}, {}, {}", transform_->GetPosition().x,
    //            transform_->GetPosition().y, transform_->GetPosition().z);

    // store the path in a local variable.
    path_to_follow_ = ai_service_->GetPath();

    transform_->SetPosition(ai_service_->GetPath()[1103]);
    // Log::debug("New Position: {}, {}, {}", transform_->GetPosition().x,
    //            transform_->GetPosition().y, transform_->GetPosition().z);

    // storing the initial variables.
    next_car_position_ = path_to_follow_[1104];
}

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
    if (speed <= 50)
    {
        vehicle_reference_->mCommandState.throttle =
            1.f * speed_multiplier_;  // for the everyone slow down pickup.
    }
    else
    {
        vehicle_reference_->mCommandState.throttle =
            0.f;  // for the everyone slow down pickup.
    }

    glm::vec3 target = -transform_->GetPosition() + next_car_position_;
    // normalize the vector to find out its true position later by dot
    // producting it
    glm::vec3 normalized_target = glm::normalize(target);
    glm::vec3 current_forward_dir = transform_->GetForwardDirection();
    float dot_product = glm::dot(normalized_target, current_forward_dir);

    // Log::debug("cross {}", cross_product.x);
    // Log::debug("dot {}", dot_product);
    // Log::debug("front {}, {}, {}", current_forward_dir.x,
    // current_forward_dir.y,
    //            current_forward_dir.z);

    if (sqrt(dot_product * dot_product) > 0.98f)
    {
        vehicle_reference_->mCommandState.steer = 0.f;
    }
    else
    {
        glm::vec3 cross_product =
            glm::normalize(glm::cross(normalized_target, current_forward_dir));
        Log::debug("cross {}, {}, {}", cross_product.x, cross_product.y,
                 cross_product.z);
        if (cross_product.x < 0)
        {
            vehicle_reference_->mCommandState.steer =
                -0.65f * handling_multiplier_;
        }
        if(cross_product.z < 0)
        {
            vehicle_reference_->mCommandState.steer =
                0.65f * handling_multiplier_;
        }
    }

    // calculate the euclidean distance to see if the car is near the next
    // position, if yes then update the next position to be the next index in
    // the path array
    float distance = glm::distance(transform_->GetPosition(),
                                   path_to_follow_[next_path_index_]);
    Log::debug("Distance to the next point {}", distance);
    if (distance < 45.f)
    {
        next_car_position_ = path_to_follow_[next_path_index_ += 1];
        // if (next_path_index_ == 91)
        // {
        //     next_path_index_ = 335;
        // }
        // else if (next_path_index_ == 129 || next_path_index_ == 530 || next_path_index_ == 496)
        // {
        //     int min_index = 0.f;
        //     float min_distance = INT_MAX;
        //     // find the smallest path which not has been traversed yet.
        //     for (int i = 0; i < path_to_follow_.size(); i++)
        //     {
        //         // if path is not traced yet.
        //         if (path_traced_.find(i) == path_traced_.end())
        //         {
        //             float dist = glm::distance(
        //                 path_to_follow_[i], path_to_follow_[next_path_index_]);
        //             if (min_distance > dist)
        //             {
        //                 min_index = i;
        //                 min_distance = dist;
        //             }
        //         }
        //     }
        //     next_path_index_ = min_index;
        // }
        // else
        // {
        //     if (path_traced_.find(next_path_index_ + 1) != path_traced_.end())
        //     {
        //         int min_index = 0.f;
        //         float min_distance = INT_MAX;
        //         // find the smallest path which not has been traversed yet.
        //         for (int i = 0; i < path_to_follow_.size(); i++)
        //         {
        //             // if path is not traced yet.
        //             if (path_traced_.find(i) == path_traced_.end())
        //             {
        //                 float dist =
        //                     glm::distance(path_to_follow_[i],
        //                                   path_to_follow_[next_path_index_]);
        //                 if (min_distance > dist)
        //                 {
        //                     min_index = i;
        //                     min_distance = dist;
        //                 }
        //             }
        //         }
        //         next_path_index_ = min_index;
        //     }
        //     else
        //     {
        //         next_car_position_ = path_to_follow_[next_path_index_ += 1];
        //     }
        // }
        // path_traced_.insert(next_path_index_);

        // if (next_path_index_ == 706)
        // {
        //     int min_index = 0.f;
        //     float min_distance = INT_MAX;
        //     // find the smallest path which not has been traversed yet.
        //     for (int i = 0; i < path_to_follow_.size(); i++)
        //     {
        //         // if path is not traced yet.
        //         if (path_traced_.find(i) == path_traced_.end())
        //         {
        //             float dist = glm::distance(
        //                 path_to_follow_[i], path_to_follow_[next_path_index_]);
        //             if (min_distance > dist)
        //             {
        //                 min_index = i;
        //                 min_distance = dist;
        //             }
        //         }
        //     }
        //     next_path_index_ = min_index;
        // }

       // Log::debug("{}", next_path_index_);
    }
}

std::string_view AIController::GetName() const
{
    return "AI Controller";
}

void AIController::SetGVehicle(snippetvehicle2::DirectDriveVehicle& vehicle)
{
    vehicle_reference_ = &vehicle;
}
