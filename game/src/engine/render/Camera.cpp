#include "engine/render/Camera.h"

#include <imgui.h>

#include <glm/gtx/transform.hpp>

#include "engine/core/debug/Log.h"
#include "engine/input/InputService.h"
#include "engine/render/RenderService.h"
#include "engine/scene/Entity.h"
#include "engine/service/ServiceProvider.h"

using glm::mat4;
using glm::vec3;

static constexpr vec3 kUpDirection(0.0f, 1.0f, 0.0f);

Camera::Camera()
    : fov_degrees_(90.0f),
      aspect_ratio_(1.0f),
      near_plane_(1.0f),
      far_plane_(200.0f),
      projection_matrix_(1.0f),
      view_matrix_(1.0f),
      render_service_(nullptr),
      input_service_(nullptr)
{
}

void Camera::OnInit(const ServiceProvider& service_provider)
{
    // Get services
    input_service_ = &service_provider.GetService<InputService>();
    render_service_ = &service_provider.GetService<RenderService>();
    render_service_->RegisterCamera(*this);

    transform_ = &GetEntity().GetComponent<Transform>();

    // Event subscriptions
    GetEventBus().Subscribe<OnUpdateEvent>(this);

    UpdateProjectionMatrix();
    UpdateViewMatrix();
}

void Camera::OnDebugGui()
{
    bool dirty = false;

    dirty |= ImGui::DragFloat("FoV (deg)", &fov_degrees_, 1.0f, 0.0f, 120.0f);
    dirty |= ImGui::DragFloat("Near Plane", &near_plane_, 1.0f, 1000.0f);
    dirty |= ImGui::DragFloat("Far Plane", &far_plane_, 1.0f, 1000.0f);

    if (dirty)
    {
        UpdateProjectionMatrix();
    }
}

std::string_view Camera::GetName() const
{
    return "Camera";
}

const mat4& Camera::GetProjectionMatrix() const
{
    return projection_matrix_;
}

const mat4& Camera::GetViewMatrix() const
{
    return view_matrix_;
}

void Camera::OnUpdate(const Timestep& delta_time)
{
    UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
    const vec3& position = transform_->GetPosition();
    const vec3& forward = transform_->GetForwardDirection();

    view_matrix_ = glm::lookAt(position, position + forward, kUpDirection);
}

void Camera::UpdateProjectionMatrix()
{
    projection_matrix_ = glm::perspective(
        glm::radians(fov_degrees_), aspect_ratio_, near_plane_, far_plane_);
}
