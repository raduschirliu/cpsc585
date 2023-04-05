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
using glm::vec4;

static constexpr vec3 kUpDirection(0.0f, 1.0f, 0.0f);
static constexpr float kDefaultFovDegrees = 90.0f;
static constexpr float kDefaultNearPlane = 0.5f;
static constexpr float kDefaultFarPlane = 1500.0f;

Camera::Camera()
    : fov_degrees_(kDefaultFovDegrees),
      aspect_ratio_(1.0f),
      near_plane_(kDefaultNearPlane),
      far_plane_(kDefaultFarPlane),
      frustum_world_(),
      type_(CameraType::kNormal),
      projection_matrix_(1.0f),
      view_matrix_(1.0f),
      inverse_view_proj_(1.0f),
      render_service_(nullptr),
      input_service_(nullptr),
      transform_(nullptr)
{
}

void Camera::OnInit(const ServiceProvider& service_provider)
{
    // Services
    input_service_ = &service_provider.GetService<InputService>();
    render_service_ = &service_provider.GetService<RenderService>();

    // Components
    transform_ = &GetEntity().GetComponent<Transform>();

    // Event subscriptions
    GetEventBus().Subscribe<OnUpdateEvent>(this);

    // Init logic
    render_service_->RegisterCamera(*this);
    UpdateProjectionMatrix();
    UpdateViewMatrix();
    UpdateFrustumVertices();
}

void Camera::OnDebugGui()
{
    bool dirty = false;

    dirty |= ImGui::DragFloat("FoV (deg)", &fov_degrees_, 1.0f, 0.0f, 120.0f);
    dirty |= ImGui::DragFloat("Near Plane", &near_plane_, 1.0f, 50000.0f);
    dirty |= ImGui::DragFloat("Far Plane", &far_plane_, 1.0f, 50000.0f);

    if (dirty)
    {
        UpdateProjectionMatrix();
    }
}

void Camera::OnDestroy()
{
    render_service_->UnregisterCamera(*this);
}

std::string_view Camera::GetName() const
{
    return "Camera";
}

void Camera::SetFov(float fov_degrees)
{
    fov_degrees_ = fov_degrees;
    UpdateProjectionMatrix();
    UpdateFrustumVertices();
}

void Camera::SetAspectRatio(float aspect_ratio)
{
    aspect_ratio_ = aspect_ratio;
    UpdateProjectionMatrix();
    UpdateFrustumVertices();
}

void Camera::SetType(CameraType type)
{
    type_ = type;
}

CameraType Camera::GetType() const
{
    return type_;
}

float Camera::GetFovDegrees() const
{
    return fov_degrees_;
}

float Camera::GetAspectRatio() const
{
    return aspect_ratio_;
}

const Cuboid& Camera::GetFrustumWorldVertices() const
{
    return frustum_world_;
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
    UpdateFrustumVertices();
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

void Camera::UpdateFrustumVertices()
{
    frustum_world_.BoundsFromNdcs(projection_matrix_ * view_matrix_);
}
