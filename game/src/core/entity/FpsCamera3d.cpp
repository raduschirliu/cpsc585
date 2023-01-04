#include "core/entity/FpsCamera3d.h"

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

static constexpr float kMoveSpeed = 15.0f;
static constexpr float kFastMoveSpeedMultiplier = 5.0f;
static constexpr float kNearPlane = 0.1f;
static constexpr float kFarPlane = 5000.0f;
static constexpr float kPitchSensitivity = 100.0f;
static constexpr float kYawSensitivity = 75.0f;

FpsCamera3d::FpsCamera3d(float fov_degrees)
    : Camera(),
      fov_radians_(glm::radians(fov_degrees)),
      pitch_degrees_(0.0f),
      yaw_degrees_(0.0f),
      last_mouse_pos_(std::nullopt),
      default_position_(vec3(0.0f, 0.0f, -10.0f)),
      default_pitch_degrees_(0.0f),
      default_yaw_degrees_(180.0f)
{
    BuildProjectionMatrix();
    Reset();
}

void FpsCamera3d::Update(float delta_time)
{
    // Reset position and orientation
    if (Input::IsKeyPressed(GLFW_KEY_R))
    {
        Reset();
    }

    // Movement
    vec3 move_dir = GetMovementDir();

    if (glm::length(move_dir) > 0)
    {
        const vec3& pos = Transform().GetPosition();
        vec3 delta = move_dir * delta_time * kMoveSpeed;

        if (Input::IsKeyDown(GLFW_KEY_LEFT_SHIFT))
        {
            delta *= kFastMoveSpeedMultiplier;
        }

        Transform().SetPosition(pos + delta);
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
                float pitch_delta = Map(glm::abs(delta.y), 0.0f,
                                        viewport_size_.y, 0.0f, 90.0f) *
                                    glm::sign(delta.y);
                float yaw_delta = Map(glm::abs(delta.x), 0.0f, viewport_size_.x,
                                      0.0f, 360.0f) *
                                  glm::sign(delta.x);

                float new_yaw =
                    yaw_degrees_ + (yaw_delta * delta_time * kYawSensitivity);
                float new_pitch = pitch_degrees_ + (pitch_delta * delta_time *
                                                    kPitchSensitivity);
                SetRotation(new_pitch, new_yaw);

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
}

void FpsCamera3d::RenderGui()
{
    if (kDrawDebugGui)
    {
        const vec3& pos = Transform().GetPosition();
        ImGui::Text("Camera pos: %f %f %f", pos.x, pos.y, pos.z);

        ImGui::Text("Camera pitch/yaw: %f %f", pitch_degrees_, yaw_degrees_);

        const vec3& fwd = Transform().GetForwardDirection();
        ImGui::Text("Camera fwd: %f %f %f", fwd.x, fwd.y, fwd.z);
    }
}

void FpsCamera3d::SetRotation(float pitch_degrees, float yaw_degrees)
{
    // Prevent camera from looking straight up/down
    pitch_degrees_ = glm::clamp(pitch_degrees, -89.0f, 89.0f);

    // Can spin 360 degrees, but wrap around at yaw >= 360 and yaw < 0
    yaw_degrees_ = glm::fmod(yaw_degrees, 360.0f);

    if (yaw_degrees_ < 0.0f)
    {
        yaw_degrees_ += 360.0f;
    }

    UpdateForwardVector();
}

void FpsCamera3d::Reset()
{
    Transform().SetPosition(default_position_);
    SetRotation(default_pitch_degrees_, default_yaw_degrees_);
}

void FpsCamera3d::SetDefaultOrientation(vec3 position, float pitch_degrees,
                                        float yaw_degrees)
{
    default_position_ = position;
    default_yaw_degrees_ = yaw_degrees;
    default_pitch_degrees_ = pitch_degrees;
}

void FpsCamera3d::BuildProjectionMatrix()
{
    projection_matrix_ =
        glm::perspective(fov_radians_, viewport_size_.x / viewport_size_.y,
                         kNearPlane, kFarPlane);
}

void FpsCamera3d::BuildViewMatrix()
{
    vec3 pos = Transform().GetPosition();
    vec3 target = pos + Transform().GetForwardDirection();
    view_matrix_ = glm::lookAt(pos, target, Transform().GetUpDirection());
}

vec3 FpsCamera3d::GetMovementDir()
{
    float forward_input = static_cast<float>(Input::IsKeyDown(GLFW_KEY_W)) -
                          static_cast<float>(Input::IsKeyDown(GLFW_KEY_S));
    float right_input = static_cast<float>(Input::IsKeyDown(GLFW_KEY_D)) -
                        static_cast<float>(Input::IsKeyDown(GLFW_KEY_A));
    float up_input = static_cast<float>(Input::IsKeyDown(GLFW_KEY_SPACE)) -
                     static_cast<float>(Input::IsKeyDown(GLFW_KEY_C));

    vec3 forward_move = Transform().GetForwardDirection() * forward_input;
    vec3 right_move = Transform().GetRightDirection() * right_input;
    vec3 up_move = Transform().GetUpDirection() * up_input;

    return forward_move + right_move + up_move;
}

// Method of finding forward vector from pitch/yaw based on the article:
// https://learnopengl.com/Getting-started/Camera
void FpsCamera3d::UpdateForwardVector()
{
    float yaw_rad = glm::radians(yaw_degrees_);
    float pitch_rad = glm::radians(pitch_degrees_);
    vec3 forward(glm::cos(yaw_rad) * glm::cos(pitch_rad), glm::sin(pitch_rad),
                 glm::sin(yaw_rad) * glm::cos(pitch_rad));

    Transform().SetForwardDirection(glm::normalize(forward));
}