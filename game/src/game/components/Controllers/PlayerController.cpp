#include "PlayerController.h"

#include "engine/core/debug/Log.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"

PlayerController::PlayerController() : vehicle_reference_(nullptr)
{
}

void PlayerController::OnInit(const ServiceProvider& service_provider)
{
    input_service_ = &service_provider.GetService<InputService>();
    transform_ = &GetEntity().GetComponent<Transform>();

    GetEventBus().Subscribe<OnUpdateEvent>(this);
}

void PlayerController::OnUpdate(const Timestep& delta_time)
{
    // the command which results into slowly slowing down the car if nothing is
    // pressed.
    executable_command_ = {0.2f, 0.0f, 0.0f, 0.0f};

    // TODO: replace all the keys and static numbers in the command to the axis
    // value we get from the joystick / button value we get from how firmly we
    // press the triggers.

    if (input_service_->IsKeyDown(GLFW_KEY_UP) ||
        input_service_->IsKeyDown(GLFW_KEY_W))
    {
        Command temp = {0.0f, 3.0f, 0.0f, timestep_};
        executable_command_ = temp;
    }
    if (input_service_->IsKeyDown(GLFW_KEY_LEFT) ||
        input_service_->IsKeyDown(GLFW_KEY_A))
    {
        Command temp = {0.0f, 0.1f, -0.4f, timestep_};
        executable_command_ = temp;
    }
    if (input_service_->IsKeyDown(GLFW_KEY_RIGHT) ||
        input_service_->IsKeyDown(GLFW_KEY_D))
    {
        Command temp = {0.0f, 0.1f, 0.4f, timestep_};
        executable_command_ = temp;
    }
    if (input_service_->IsKeyDown(GLFW_KEY_DOWN) ||
        input_service_->IsKeyDown(GLFW_KEY_S))
    {
        Command temp = {1.0f, 0.0f, 0.0f, timestep_};
        executable_command_ = temp;
    }

    vehicle_reference_->mCommandState.brakes[0] = executable_command_.brake;
    vehicle_reference_->mCommandState.nbBrakes = 1;
    vehicle_reference_->mCommandState.throttle = executable_command_.throttle;
    vehicle_reference_->mCommandState.steer = executable_command_.steer;
}

std::string_view PlayerController::GetName() const
{
    return "Player Controller";
}