#include "game/components/DebugCameraController.h"

#include "engine/core/debug/Log.h"
#include "engine/scene/Entity.h"

using glm::vec3;
using std::string_view;

static float kMoveSpeed = 0.25f;

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
    vec3 pos_delta(0.0f, 0.0f, 0.0f);

    if (input_service_->IsKeyDown(GLFW_KEY_A))
    {
        pos_delta.x += kMoveSpeed;
    }
    if (input_service_->IsKeyDown(GLFW_KEY_D))
    {
        pos_delta.x -= kMoveSpeed;
    }
    if (input_service_->IsKeyDown(GLFW_KEY_W))
    {
        pos_delta.z += kMoveSpeed;
    }
    if (input_service_->IsKeyDown(GLFW_KEY_S))
    {
        pos_delta.z -= kMoveSpeed;
    }

    transform_->Translate(pos_delta);
}