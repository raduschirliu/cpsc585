#include "game/components/FollowCamera.h"

#include <glm/gtx/transform.hpp>

#include "engine/core/debug/Log.h"
#include "engine/core/gui/PropertyWidgets.h"
#include "engine/input/InputService.h"
#include "engine/render/Camera.h"
#include "engine/render/RenderService.h"
#include "engine/scene/Entity.h"
#include "engine/service/ServiceProvider.h"
#include "game/components/VehicleComponent.h"
#include "game/components/state/PlayerState.h"

using namespace glm;
using glm::quat;
using glm::vec3;

FollowCamera::FollowCamera()
    : offset_(0.0f, 10.0f, 0.0f),
      distance_(23.0f),
      orientation_lerp_factor_(3.5f),
      position_lerp_factor_(3.5f),
      acceleration_factor_(15.f)
{
}

void FollowCamera::OnInit(const ServiceProvider& service_provider)
{
    // Dependencies
    transform_ = &GetEntity().GetComponent<Transform>();

    // Event subscriptions
    GetEventBus().Subscribe<OnUpdateEvent>(this);
}

void FollowCamera::SetFollowingTransform(Entity& entity)
{
    target_transform_ = &entity.GetComponent<Transform>();
    target_vehicle_ = &entity.GetComponent<VehicleComponent>();
}

std::string_view FollowCamera::GetName() const
{
    return "Follow Camera";
}

bool activate = true;

void FollowCamera::OnUpdate(const Timestep& delta_time)
{
    const quat target_orientation = target_transform_->GetOrientation();

    // Calculate the target position with offset and distance
    vec3 target_position =
        target_transform_->GetPosition() +
        target_transform_->GetForwardDirection() * -distance_ + offset_;

    const float dt_sec = static_cast<float>(delta_time.GetSeconds());

    if (activate && player_state_ && player_state_->GetIsAccelerating())
    {
        // Modify the target_position to tilt up/down
        float tilt_angle = 10.0f;  // Adjust the tilt angle as needed
        float tilt_amount = sin(glm::radians(tilt_angle)) * acceleration_factor_; // Adjust the acceleration factor as needed
        target_position.y += tilt_amount;
    }

    // Apply the lerping with the modified target position and original target orientation
    transform_->SlerpOrientation(target_orientation, orientation_lerp_factor_ * dt_sec);
    transform_->LerpPosition(target_position, position_lerp_factor_ * dt_sec);
}



void FollowCamera::SetPlayerState(PlayerState& player_state)
{
    player_state_ = &player_state;
}

void FollowCamera::OnDebugGui()
{
    ImGui::Checkbox("Activate the interpolation", &activate);
    ImGui::DragFloat("Acceleration Factor", &acceleration_factor_, 0.1f, -0.0f, 100.0f);
    gui::EditProperty("Offset", offset_);
    ImGui::DragFloat("Distance", &distance_, 1.0f, -100.0f, 100.0f);
    ImGui::DragFloat("Orientation Lerp Factor", &orientation_lerp_factor_,
                     0.05f, 0.0f, 10.0f);
    ImGui::DragFloat("Position Lerp Factor", &position_lerp_factor_, 0.05f,
                     0.0f, 10.0f);
}