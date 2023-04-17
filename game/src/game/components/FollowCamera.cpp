#include "game/components/FollowCamera.h"

#include <glm/gtx/transform.hpp>

#include "engine/core/debug/Log.h"
#include "engine/core/gui/PropertyWidgets.h"
#include "engine/input/InputService.h"
#include "engine/render/Camera.h"
#include "engine/render/RenderService.h"
#include "engine/scene/Entity.h"
#include "engine/scene/Transform.h"
#include "engine/service/ServiceProvider.h"
#include "game/components/VehicleComponent.h"
#include "game/components/state/PlayerState.h"

using namespace glm;
using glm::quat;
using glm::vec3;
using glm::vec4;

static constexpr float kBaseFovDeg = 90.0f;
static constexpr float kMaxFovDeg = 105.0f;

FollowCamera::FollowCamera()
    : offset_(0.0f, 8.0f, 0.0f),
      lookat_offset_(0.0f, 5.0f, 0.0f),
      distance_(25.0f),
      orientation_lerp_factor_(20.0f),
      position_lerp_factor_(5.0f),
      acceleration_factor_(20.0f),
      fov_factor_(50.0f),
      fov_offset_(0.0f)
{
}

void FollowCamera::OnInit(const ServiceProvider& service_provider)
{
    // Dependencies
    transform_ = &GetEntity().GetComponent<Transform>();
    camera_ = &GetEntity().GetComponent<Camera>();

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
    {
        return;
    }

    // Calculate the target position with offset and distance
    vec3 camera_position =
        target_transform_->GetPosition() +
        target_transform_->GetForwardDirection() * -distance_ + offset_;

    vec3 target_position = target_transform_->GetPosition() + lookat_offset_;

    const float dt_sec = static_cast<float>(delta_time.GetSeconds());

    // Modify the target_position to tilt up/down
    const float tilt_angle =
        20.0f * (player_state_->GetCurrentSpeed() / 130.0f);
    const float oscillation = sin(glm::radians(tilt_angle));
    const float fov_increase =
        glm::clamp(oscillation * fov_factor_ - fov_offset_, 0.0f,
                   kMaxFovDeg - kBaseFovDeg);
    const float tilt_amount = oscillation * acceleration_factor_;

    target_position.y += tilt_amount;

    quat target_orientation =
        glm::quatLookAt(-normalize(target_position - transform_->GetPosition()),
                        vec3(0.0f, 1.0f, 0.0f));

    if (glm::any(glm::isnan(target_orientation)))
    {
        target_orientation = transform_->GetOrientation();
    }

    // Apply the lerping with the modified target position and original target
    // orientation
    transform_->SlerpOrientation(target_orientation,
                                 orientation_lerp_factor_ * dt_sec);
    transform_->LerpPosition(camera_position, position_lerp_factor_ * dt_sec);

    camera_->SetFov(kBaseFovDeg + fov_increase);
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
    gui::EditProperty("Lookat Offset", lookat_offset_);
    ImGui::DragFloat("Distance", &distance_, 1.0f, -100.0f, 100.0f);
    ImGui::DragFloat("Fov Factor", &fov_factor_, 1.0f, -100.0f, 100.0f);
    ImGui::DragFloat("Fov Offset", &fov_offset_, 1.0f, -100.0f, 100.0f);
    ImGui::DragFloat("Orientation Lerp Factor", &orientation_lerp_factor_,
                     0.05f, 0.0f, 10.0f);
    ImGui::DragFloat("Position Lerp Factor", &position_lerp_factor_, 0.05f,
                     0.0f, 10.0f);
}