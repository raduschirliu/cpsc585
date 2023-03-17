#include "PlayerController.h"

#include <imgui.h>

#include "engine/core/Colors.h"
#include "engine/core/debug/Log.h"
#include "engine/core/math/Common.h"
#include "engine/physics/PhysicsService.h"
#include "engine/render/RenderService.h"
#include "engine/scene/Entity.h"
#include "game/components/state/PlayerState.h"

static constexpr size_t kGamepadId = GLFW_JOYSTICK_1;
static constexpr float kDefaultBrake = -0.1f;
static constexpr float kSpeedMultiplier = 1.0f;
static constexpr float kHanldingMultiplier = 1.0f;

void PlayerController::OnInit(const ServiceProvider& service_provider)
{
    input_service_ = &service_provider.GetService<InputService>();
    game_state_service_ = &service_provider.GetService<GameStateService>();

    transform_ = &GetEntity().GetComponent<Transform>();
    player_data_ = &GetEntity().GetComponent<PlayerState>();
    vehicle_ = &GetEntity().GetComponent<VehicleComponent>();

    GetEventBus().Subscribe<OnUpdateEvent>(this);
}

void PlayerController::OnUpdate(const Timestep& delta_time)
{
    if (game_state_service_->GetRaceState()
            .countdown_elapsed_time.GetSeconds() <=
        game_state_service_->GetMaxCountdownSeconds())
        return;
    UpdatePowerupControls(delta_time);
    UpdateCarControls(delta_time);
}

std::string_view PlayerController::GetName() const
{
    return "Player Controller";
}

void PlayerController::UpdatePowerupControls(const Timestep& delta_time)
{
    if (input_service_->IsKeyDown(GLFW_KEY_SPACE))
    {
        if (player_data_->GetCurrentPowerup() ==
            PowerupPickupType::kDefaultPowerup)
        {
            Log::debug("You currently do not have any powerup.");
        }
        else
        {
            // Log::debug("executing the powerup");
            // power executed, so add it to the map in game service.
            game_state_service_->AddPlayerPowerup(
                GetEntity().GetId(), player_data_->GetCurrentPowerup());
        }
    }

    // this means that the everyone slower pickup is active right now.
    if (uint32_t id =
            game_state_service_->GetEveryoneSlowerSpeedMultiplier() != NULL)
    {
        // now except for the entity who launched it, all the entities should
        // slow down.
        if (GetEntity().GetId() != id)
        {
            // if any AI picked up the powerup then the player's speed should be
            // reduced.
            speed_multiplier_ = 0.2f;
        }
        else
        {
            // this is the entity which started the powerup, so do nothing.
        }
    }
    else
    {
        speed_multiplier_ = kSpeedMultiplier;
    }

    if (uint32_t id =
            game_state_service_->GetDisableHandlingMultiplier() != NULL)
    {
        // now except for the entity who launched it, all the entities should
        // slow down.
        if (GetEntity().GetId() != id)
        {
            // if any AI picked up the powerup then the player's speed should be
            // reduced.
            handling_multiplier_ = 0.0f;
        }
        else
        {
            // this is the entity which started the powerup, so do nothing.
            handling_multiplier_ = kHanldingMultiplier;
        }
    }
    else
    {
        speed_multiplier_ = kSpeedMultiplier;
    }
}

void PlayerController::UpdateCarControls(const Timestep& delta_time)
{
    // TODO: Need to take multipliers into account again
    // (or move them into VehicleComponent instead)

    UpdateGear();
    command_.steer = GetSteerDirection();
    command_.throttle = GetThrottle();
    command_.front_brake = GetFrontBrake();
    vehicle_->SetCommand(command_);
}

float PlayerController::GetSteerDirection()
{
    if (input_service_->IsKeyDown(GLFW_KEY_A))
    {
        return 1.0f;
    }
    else if (input_service_->IsKeyDown(GLFW_KEY_D))
    {
        return -1.0f;
    }

    if (input_service_->IsGamepadActive(kGamepadId))
    {
        return -input_service_->GetGamepadAxis(kGamepadId,
                                               GLFW_GAMEPAD_AXIS_LEFT_X);
    }

    return 0.0f;
}

float PlayerController::GetThrottle()
{
    if (input_service_->IsKeyDown(GLFW_KEY_W))
    {
        return 1.0f;
    }
    else if (input_service_->IsKeyDown(GLFW_KEY_S))
    {
        return 0.0f;
    }

    if (input_service_->IsGamepadActive(kGamepadId))
    {
        return math::Map(input_service_->GetGamepadAxis(
                             kGamepadId, GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER),
                         -1.0f, 1.0f, 0.0f, 1.0f);
    }

    return 0.0f;
}

void PlayerController::UpdateGear()
{
    if (GetGearChangeButton())
    {
        forward_gear_ = !forward_gear_;

        if (forward_gear_)
        {
            Log::info("Switched into drive gear");
            vehicle_->SetGear(VehicleGear::kForward);
        }
        else
        {
            Log::info("Switched into reverse gear");
            vehicle_->SetGear(VehicleGear::kReverse);
        }
    }
}

float PlayerController::GetFrontBrake()
{
    if (input_service_->IsKeyDown(GLFW_KEY_S))
    {
        return 1.0f;
    }

    if (input_service_->IsGamepadActive(kGamepadId))
    {
        const float gamepad_trigger =
            math::Map(input_service_->GetGamepadAxis(
                          kGamepadId, GLFW_GAMEPAD_AXIS_LEFT_TRIGGER),
                      -1.0f, 1.0f, 0.0f, 1.0f);

        return glm::max(gamepad_trigger, kDefaultBrake);
    }

    return 0.0f;
}

void PlayerController::OnDebugGui()
{
    ImGui::Text("Reverse: %d", !forward_gear_);
    ImGui::Text("Steer: %f", command_.steer);
    ImGui::Text("Throttle: %f", command_.throttle);
    ImGui::Text("Front Brake: %f", command_.front_brake);
    ImGui::Text("Rear Brake: %f", command_.rear_brake);
}

bool PlayerController::GetGearChangeButton()
{
    return input_service_->IsKeyPressed(GLFW_KEY_X) ||
           input_service_->IsGamepadButtonPressed(kGamepadId,
                                                  GLFW_GAMEPAD_BUTTON_X);
}