#include "game/components/DebugCameraController.h"

#include "engine/core/debug/Log.h"
#include "engine/core/math/Common.h"
#include "engine/scene/Entity.h"

using glm::vec2;
using glm::vec3;
using std::string_view;

static constexpr float kMoveSpeed = 0.25f;
static constexpr float kFastMoveSpeedMultiplier = 2.0f;
static constexpr float kPitchSensitivity = 5.0f;
static constexpr float kYawSensitivity = 2.5f;

// TODO: Don't hardcode this...
static constexpr vec2 kViewportSize(1280.0f, 720.0f);

DebugCameraController::DebugCameraController()
    : transform_(nullptr),
      input_service_(nullptr),
      last_mouse_pos_(std::nullopt),
      pitch_degrees_(0.0f),
      yaw_degrees_(0.0f)
{
}

void DebugCameraController::OnInit(const ServiceProvider& service_provider)
{
    Log::info("DebugCameraController - Init");

    // Dependencies
    input_service_ = &service_provider.GetService<InputService>();
    transform_ = &GetEntity().GetComponent<Transform>();

    // Events
    GetEventBus().Subscribe<OnUpdateEvent>(this);
}

string_view DebugCameraController::GetName() const
{
    return "DebugCameraController";
}

void DebugCameraController::OnUpdate()
{
    // Movement
    const vec3 move_dir = GetMovementDir();

    if (glm::length(move_dir) > 0)
    {
        const vec3& pos = Transform().GetPosition();
        vec3 delta = move_dir * kMoveSpeed;

        if (input_service_->IsKeyDown(GLFW_KEY_LEFT_SHIFT))
        {
            delta *= kFastMoveSpeedMultiplier;
        }

        transform_->Translate(delta);
    }

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
                    glm::radians(pitch_delta * kPitchSensitivity),
                    transform_->GetRightDirection()));

                // Yaw rotates around vertical axis
                transform_->Rotate(
                    glm::angleAxis(glm::radians(-yaw_delta * kYawSensitivity),
                                   vec3(0.0f, 1.0f, 0.0f)));

                last_mouse_pos_ = mouse_pos;
            }
        }
        else
        {
            last_mouse_pos_ = input_service_->GetMousePos();
        }
    }

    if (input_service_->IsMouseButtonReleased(GLFW_MOUSE_BUTTON_1))
    {
        last_mouse_pos_.reset();
    }
}

vec3 DebugCameraController::GetMovementDir()
{
    float forward_input =
        static_cast<float>(input_service_->IsKeyDown(GLFW_KEY_W)) -
        static_cast<float>(input_service_->IsKeyDown(GLFW_KEY_S));
    float up_input =
        static_cast<float>(input_service_->IsKeyDown(GLFW_KEY_SPACE)) -
        static_cast<float>(input_service_->IsKeyDown(GLFW_KEY_C));
    float right_input =
        static_cast<float>(input_service_->IsKeyDown(GLFW_KEY_A)) -
        static_cast<float>(input_service_->IsKeyDown(GLFW_KEY_D));

    vec3 forward_move = transform_->GetForwardDirection() * forward_input;
    vec3 up_move = transform_->GetUpDirection() * up_input;
    vec3 right_move = transform_->GetRightDirection() * right_input;

    return forward_move + right_move + up_move;
}