#include "AIController.h"

#include "engine/core/debug/Log.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"

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
    GetEventBus().Subscribe<OnUpdateEvent>(this);

    if (ai_service_)
    {
        // store the path in a local variable.
        path_to_follow_ = ai_service_->GetPath();
    }

    // storing the initial variables.
    if (path_to_follow_.size() > 2)
    {
        next_car_position_ = path_to_follow_[1];
    }
}

void AIController::OnUpdate(const Timestep& delta_time)
{
    /**
     * Here we have different criteria on how we want the car to move, the more
     * advanced this is, the better the car will move on the track
     * **/
    // Log::debug("{}",
    // vehicle_reference_->mPhysXState.physxActor.rigidBody->getGlobalPose().p);
    vehicle_reference_->mCommandState.throttle = 1.f;
    glm::vec3 target = transform_->GetPosition() - next_car_position_;
    // normalize the vector to find out its true position later by dot
    // producting it
    glm::vec3 normalized_target = glm::normalize(target);
    glm::vec3 current_forward_dir = transform_->GetForwardDirection();
    float dot_product = (normalized_target.x * current_forward_dir.x +
                         normalized_target.y * current_forward_dir.y +
                         normalized_target.z * current_forward_dir.z);

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
            vehicle_reference_->mCommandState.steer = 0.4f;
        }
        else
        {
            vehicle_reference_->mCommandState.steer = -0.4f;
        }
    }

    // calculate the euclidean distance to see if the car is near the next
    // position, if yes then update the next position to be the next index in
    // the path array
    if (GetEuclideanDistance(transform_->GetPosition(),
                             path_to_follow_[next_path_index_]) < 2.f)
    {
        next_car_position_ = path_to_follow_[next_path_index_++];
    }
}

std::string_view AIController::GetName() const
{
    return "AI Controller";
}