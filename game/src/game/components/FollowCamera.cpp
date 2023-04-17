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

void FollowCamera::OnUpdate(const Timestep& delta_time)
{
    if (!player_state_)
        return;

    // Calculate the target position with offset and distance
    vec3 target_position =
        target_transform_->GetPosition() +
        target_transform_->GetForwardDirection() * -distance_ + offset_;

    auto target_orientation = target_transform_->GetOrientation();

    const float dt_sec = static_cast<float>(delta_time.GetSeconds());

    // Modify the target_position to tilt up/down
    float tilt_angle = 20.0f * (player_state_->GetCurrentSpeed() / 130.f);
    float oscillation = sin(glm::radians(tilt_angle));
    float tilt_amount = oscillation * acceleration_factor_;

    target_position.y += tilt_amount;

    // Apply the lerping with the modified target position and original target
    // orientation
    transform_->SlerpOrientation(target_orientation,
                                 orientation_lerp_factor_ * dt_sec);
    transform_->LerpPosition(target_position, position_lerp_factor_ * dt_sec);
}

void FollowCamera::SetPlayerState(PlayerState& player_state)
{
    player_state_ = &player_state;
}

void FollowCamera::OnDebugGui()
{
    // ImGui::Checkbox("Activate the interpolation", &activate);
    ImGui::Text("Speed = %f", player_state_->GetCurrentSpeed());
    ImGui::DragFloat("Acceleration Factor", &acceleration_factor_, 0.1f, -0.0f,
                     100.0f);
    gui::EditProperty("Offset", offset_);
    ImGui::DragFloat("Distance", &distance_, 1.0f, -100.0f, 100.0f);
    ImGui::DragFloat("Orientation Lerp Factor", &orientation_lerp_factor_,
                     0.05f, 0.0f, 10.0f);
    ImGui::DragFloat("Position Lerp Factor", &position_lerp_factor_, 0.05f,
                     0.0f, 10.0f);
}