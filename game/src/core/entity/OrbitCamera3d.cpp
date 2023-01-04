#include "core/entity/OrbitCamera3d.h"

#include <imgui.h>

#include <glm/gtc/matrix_transform.hpp>

#include "core/Input.h"
#include "core/Utils.h"
#include "core/gfx/Log.h"

using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using std::shared_ptr;
using utils::kIdentityMatrix;
using utils::Map;

// Adds some extra info in the UI about the camera position, rotation
// and forward vector
static constexpr bool kDrawDebugGui = false;

static constexpr float kNearPlane = 0.1f;
static constexpr float kFarPlane = 20000.0f;
static constexpr float kRadiusZoomSpeed = 80.0f;

// Never want to look straight up or straight down
static constexpr float kThetaMax = glm::pi<float>() - 0.1f;
static constexpr float kThetaMin = 0.1f;
static constexpr float kThetaSensitivity = 0.5f;

static constexpr float kPhiMin = 0.0f;
static constexpr float kPhiMax = glm::two_pi<float>();
static constexpr float kPhiSensitivity = 1.0f;

OrbitCamera3d::OrbitCamera3d(float fov_degrees, float orbit_radius)
    : Camera(),
      fov_radians_(glm::radians(fov_degrees)),
      theta_rad_(0.0f),
      phi_rad_(0.0f),
      orbit_radius_(orbit_radius),
      default_orbit_radius_(orbit_radius),
      last_mouse_pos_(std::nullopt)
{
    BuildProjectionMatrix();
    Reset();
}

void OrbitCamera3d::Update(float delta_time)
{
    // Reset position and orientation
    if (Input::IsKeyPressed(GLFW_KEY_R))
    {
        Reset();
    }

    // Rotation
    if (Input::IsMouseButtonDown(GLFW_MOUSE_BUTTON_1))
    {
        if (last_mouse_pos_)
        {
            vec2 mouse_pos = Input::GetMousePos();
            vec2 delta = mouse_pos - last_mouse_pos_.value();

            if (glm::length(delta) > 0)
            {
                // Map from mouse pos to degrees of rotation
                float theta_delta = Map(glm::abs(delta.y), 0.0f,
                                        viewport_size_.y, 0.0f, 180.0f) *
                                    glm::sign(delta.y);
                float phi_delta = Map(glm::abs(delta.x), 0.0f, viewport_size_.x,
                                      0.0f, 360.0f) *
                                  glm::sign(delta.x);

                Rotate(glm::radians(phi_delta) * kPhiSensitivity,
                       glm::radians(theta_delta) * kThetaSensitivity);

                last_mouse_pos_ = mouse_pos;
            }
        }
        else
        {
            last_mouse_pos_ = Input::GetMousePos();
        }
    }

    if (Input::IsMouseButtonReleased(GLFW_MOUSE_BUTTON_1))
    {
        last_mouse_pos_.reset();
    }

    // Changing orbit radius
    float orbit_radius_delta =
        delta_time * kRadiusZoomSpeed * GetZoomChangeDir();

    orbit_radius_ += orbit_radius_delta;

    // Update position
    vec3 new_pos =
        utils::SphericalToCartesian(orbit_radius_, phi_rad_, theta_rad_);
    Transform().SetPosition(new_pos);
}

void OrbitCamera3d::RenderGui()
{
    if (kDrawDebugGui)
    {
        const vec3& pos = Transform().GetPosition();
        ImGui::Text("Camera pos: %f %f %f", pos.x, pos.y, pos.z);

        ImGui::Text("Camera phi/theta/radius: %f %f %f", phi_rad_, theta_rad_,
                    orbit_radius_);

        const vec3& fwd = Transform().GetForwardDirection();
        ImGui::Text("Camera fwd: %f %f %f", fwd.x, fwd.y, fwd.z);
    }
}

void OrbitCamera3d::Reset()
{
    phi_rad_ = glm::pi<float>();
    theta_rad_ = glm::quarter_pi<float>();
    orbit_radius_ = default_orbit_radius_;
}

void OrbitCamera3d::BuildProjectionMatrix()
{
    projection_matrix_ =
        glm::perspective(fov_radians_, viewport_size_.x / viewport_size_.y,
                         kNearPlane, kFarPlane);
}

void OrbitCamera3d::BuildViewMatrix()
{
    vec3 pos = Transform().GetPosition();
    vec3 target(0.0f, 0.0f, 0.0f);
    view_matrix_ = glm::lookAt(pos, target, Transform().GetUpDirection());
}

void OrbitCamera3d::Rotate(float phi_delta, float theta_delta)
{
    if (theta_rad_ + theta_delta > kThetaMin &&
        theta_rad_ + theta_delta < kThetaMax)
    {
        theta_rad_ += theta_delta;
    }

    phi_rad_ -= phi_delta;

    if (phi_rad_ > kPhiMax)
    {
        phi_rad_ -= kPhiMax;
    }
    else if (phi_rad_ < kPhiMin)
    {
        phi_rad_ += kPhiMax;
    }
}

float OrbitCamera3d::GetZoomChangeDir()
{
    // Zooming in = decreasing radius
    float dir = static_cast<float>(Input::IsKeyDown(GLFW_KEY_S)) -
                static_cast<float>(Input::IsKeyDown(GLFW_KEY_W));

    if (Input::IsKeyDown(GLFW_KEY_LEFT_SHIFT))
    {
        dir *= 2.0f;
    }

    return dir;
}