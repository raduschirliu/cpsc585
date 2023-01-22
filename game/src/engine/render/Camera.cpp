#include "engine/render/Camera.h"

#include <glm/gtx/transform.hpp>

#include "engine/input/InputService.h"
#include "engine/render/RenderService.h"
#include "engine/service/ServiceProvider.h"

using glm::mat4;
using glm::vec3;

// TEMP: to remove after we have Transforms
static float kMoveSpeed = 0.25f;

Camera::Camera()
    : fov_degrees_(90.0f),
      aspect_ratio_(1.0f),
      near_plane_(1.0f),
      far_plane_(200.0f),
      projection_matrix_(1.0f),
      view_matrix_(1.0f),
      render_service_(nullptr),
      input_service_(nullptr),
      position_(0.0f, 0.0f, -5.0f)
{
}

void Camera::OnInit(const ServiceProvider& service_provider)
{
    // Get services
    render_service_ = &service_provider.GetService<RenderService>();
    render_service_->RegisterCamera(*this);

    input_service_ = &service_provider.GetService<InputService>();

    // Event subscriptions
    GetEventBus().Subscribe<OnUpdateEvent>(this);

    // Setup camera params
    projection_matrix_ = glm::perspective(
        glm::radians(fov_degrees_), aspect_ratio_, near_plane_, far_plane_);

    UpdateViewMatrix();
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

void Camera::OnUpdate()
{
    // TODO(radu): This should be moved to its own component once we have a
    // Transform component
    if (input_service_->IsKeyDown(GLFW_KEY_A))
    {
        position_.x += kMoveSpeed;
    }
    if (input_service_->IsKeyDown(GLFW_KEY_D))
    {
        position_.x -= kMoveSpeed;
    }
    if (input_service_->IsKeyDown(GLFW_KEY_W))
    {
        position_.z += kMoveSpeed;
    }
    if (input_service_->IsKeyDown(GLFW_KEY_S))
    {
        position_.z -= kMoveSpeed;
    }

    UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
    // TODO(radu): Don't hardcode these, get them from Transform component
    view_matrix_ = glm::lookAt(position_, position_ + vec3(0.0f, 0.0f, 1.0f),
                               vec3(0.0f, 1.0f, 0.0f));
}