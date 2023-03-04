#include "AIController.h"

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

    // store the path in a local variable.
    path_to_follow_ = ai_service_->GetPath();

    // storing the initial variables.
    if (path_to_follow_.size() > 2)
    {
        next_car_position_ = path_to_follow_[1];
    }
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

    vehicle_reference_->mCommandState.throttle =
        0.1f * speed_multiplier_;  // for the everyone slow down pickup.
    glm::vec3 target = transform_->GetPosition() - next_car_position_;
    // normalize the vector to find out its true position later by dot
    // producting it
    glm::vec3 normalized_target = glm::normalize(target);
    glm::vec3 current_forward_dir = transform_->GetForwardDirection();
    float dot_product = glm::dot(normalized_target, current_forward_dir);

    // Log::debug("Player ID: {} ; speed: {}", GetEntity().GetName(),
    // speed_multiplier_);

    if (sqrt(dot_product * dot_product) > 0.95f)
    {
        vehicle_reference_->mCommandState.steer = 0.f;
    }
    else
    {
        glm::vec3 cross_product =
            glm::cross(current_forward_dir, normalized_target);
        if (cross_product.x < 0)
        {
            vehicle_reference_->mCommandState.steer =
                -0.6f * handling_multiplier_;
        }
        else
        {
            vehicle_reference_->mCommandState.steer =
                0.6f * handling_multiplier_;
        }
    }

    // calculate the euclidean distance to see if the car is near the next
    // position, if yes then update the next position to be the next index in
    // the path array
    float distance = glm::distance(transform_->GetPosition(),
                                   path_to_follow_[next_path_index_]);
    if (distance < 7.f)
    {
        next_car_position_ = path_to_follow_[next_path_index_++];
        //Log::debug("{}", next_path_index_);
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
