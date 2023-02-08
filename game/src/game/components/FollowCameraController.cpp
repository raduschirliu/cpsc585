#include "FollowCameraController.h"

#include "engine/core/math/Common.h"

using namespace glm;

static constexpr float kMoveSpeed = 15.0f;
static constexpr float kFastMoveSpeedMultiplier = 2.0f;
static constexpr float kPitchSensitivity = 160.0f;
static constexpr float kYawSensitivity = 90.0f;

static constexpr vec2 kViewportSize(1280.0f, 720.0f);

void FollowCameraController::OnUpdate(const Timestep& delta_time)
{
    // We do not want the movement using WASD so not calling the super
    // DebugCameraController::OnUpdate(delta_time);

    const float delta_time_sec = static_cast<float>(delta_time.GetSeconds());

    if (input_service_->IsMouseButtonDown(GLFW_MOUSE_BUTTON_1))
    {
        if (last_mouse_pos_)
        {
            vec2 mouse_pos = input_service_->GetMousePos();
            vec2 delta = mouse_pos - last_mouse_pos_.value();

            if (glm::length(delta) > 0)
            {
                // Map from mouse pos to degrees of rotation
                const float pitch_delta =
                    math::Map(glm::abs(delta.y), 0.0f, kViewportSize.y, 0.0f,
                              90.0f) *
                    glm::sign(delta.y);
                const float yaw_delta =
                    math::Map(glm::abs(delta.x), 0.0f, kViewportSize.x, 0.0f,
                              360.0f) *
                    glm::sign(delta.x);

                // Pitch rotates around horizontal axis
                transform_->Rotate(glm::angleAxis(
                    glm::radians(pitch_delta * kPitchSensitivity *
                                 delta_time_sec),
                    transform_->GetRightDirection()));

                // Yaw rotates around vertical axis
                transform_->Rotate(glm::angleAxis(
                    glm::radians(-yaw_delta * kYawSensitivity * delta_time_sec),
                    vec3(0.0f, 1.0f, 0.0f)));

                last_mouse_pos_ = mouse_pos;
            }
        }
        else
        {
            //last_mouse_pos_ = input_service_->GetMousePos();
            last_mouse_pos_ = input_service_->GetMousePos();
        }
    }

    if (input_service_->IsMouseButtonReleased(GLFW_MOUSE_BUTTON_1))
    {
        last_mouse_pos_.reset();
    }
}