#include "game/components/FollowCamera.h"

#include <glm/gtx/transform.hpp>

#include "engine/core/debug/Log.h"
#include "engine/core/gui/PropertyWidgets.h"
#include "engine/input/InputService.h"
#include "engine/render/Camera.h"
#include "engine/render/RenderService.h"
#include "engine/scene/Entity.h"
#include "engine/service/ServiceProvider.h"

using namespace glm;
using glm::quat;
using glm::vec3;

FollowCamera::FollowCamera()
    : offset_(0.0f, 10.0f, 0.0f),
      distance_(30.0f),
      orientation_lerp_factor_(0.95f),
      position_lerp_factor_(1.1f)
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
}

std::string_view FollowCamera::GetName() const
{
    return "Follow Camera";
}

void FollowCamera::OnUpdate(const Timestep& delta_time)
{
    quat target_orientation = target_transform_->GetOrientation();
    vec3 target_position =
        target_transform_->GetPosition() +
        target_transform_->GetForwardDirection() * -distance_ + offset_;

    transform_->SlerpOrientation(
        target_orientation, orientation_lerp_factor_ * delta_time.GetSeconds());
    transform_->LerpPosition(target_position,
                             position_lerp_factor_ * delta_time.GetSeconds());
}

void FollowCamera::OnDebugGui()
{
    gui::EditProperty("Offset", offset_);
    ImGui::DragFloat("Distance", &distance_, 1.0f, -100.0f, 100.0f);
    ImGui::DragFloat("Orientation Lerp Factor", &orientation_lerp_factor_,
                     0.05f, 0.0f, 10.0f);
    ImGui::DragFloat("Position Lerp Factor", &position_lerp_factor_, 0.05f,
                     0.0f, 10.0f);
    ImGui::Spacing();
}