#include "PlayerController.h"

#include <imgui.h>

#include "engine/App.h"
#include "engine/core/Colors.h"
#include "engine/core/debug/Log.h"
#include "engine/core/math/Common.h"
#include "engine/input/InputService.h"
#include "engine/physics/PhysicsService.h"
#include "engine/pickup/PickupService.h"
#include "engine/render/RenderService.h"
#include "engine/scene/Entity.h"
#include "engine/scene/Transform.h"
#include "game/components/VehicleComponent.h"
#include "game/components/shooting/Shooter.h"
#include "game/components/state/PlayerState.h"
#include "game/services/GameStateService.h"

static constexpr size_t kGamepadId = GLFW_JOYSTICK_1;
static constexpr float kRespawnSeconds = 3.0f;
static constexpr float kDefaultBrake = 0.0f;
static constexpr float kSpeedMultiplier = 0.1f;
static constexpr float kHandlingMultiplier = 0.0f;
static float shoot_cooldown;

void PlayerController::OnInit(const ServiceProvider& service_provider)
{
    input_service_ = &service_provider.GetService<InputService>();
    game_state_service_ = &service_provider.GetService<GameStateService>();
    pickup_service_ = &service_provider.GetService<PickupService>();

    transform_ = &GetEntity().GetComponent<Transform>();
    player_data_ = &GetEntity().GetComponent<PlayerState>();
    vehicle_ = &GetEntity().GetComponent<VehicleComponent>();
    shooter_ = &GetEntity().GetComponent<Shooter>();
    
    shoot_cooldown = 0.0f;

    GetEventBus().Subscribe<OnUpdateEvent>(this);
}

void PlayerController::OnUpdate(const Timestep& delta_time)
{
    if (game_state_service_->GetRaceState()
            .countdown_elapsed_time.GetSeconds() <=
        game_state_service_->GetMaxCountdownSeconds())
    {
        return;
    }

    if (player_data_->IsDead())
    {
        return;
    }
    UpdatePowerupControls(delta_time);
    UpdateCarControls(delta_time);
    CheckShoot(delta_time);
}

std::string_view PlayerController::GetName() const
{
    return "Player Controller";
}

void PlayerController::CheckShoot(const Timestep& delta_time)
{
    if (shoot_cooldown > 0.0f)
    {
        shoot_cooldown -= static_cast<float>(delta_time.GetSeconds());
        return;
    }

    if (input_service_->IsKeyPressed(GLFW_KEY_R) ||
        input_service_->IsGamepadButtonPressed(kGamepadId,
                                               GLFW_GAMEPAD_BUTTON_B))
    {
        shooter_->Shoot();
        shoot_cooldown = shooter_->GetCooldownTime();
    }
}

void PlayerController::UpdatePowerupControls(const Timestep& delta_time)
{
    if (input_service_->IsKeyDown(GLFW_KEY_SPACE))
    {
        if (player_data_->GetCurrentPowerup() ==
            PowerupPickupType::kDefaultPowerup)
        {
            return;
        }
        else
        {
            switch (player_data_->GetCurrentPowerup())
            {
                case PowerupPickupType::kDisableHandling:
                    // handle executing the powerup
                    pickup_service_->AddEntityWithPowerup(&GetEntity(),
                                                          "DisableHandling");
                    pickup_service_->AddEntityWithTimer(&GetEntity(), 0.0f);
                    break;

                case PowerupPickupType::kEveryoneSlower:
                    // handle executing the powerup
                    pickup_service_->AddEntityWithPowerup(&GetEntity(),
                                                          "EveryoneSlower");
                    pickup_service_->AddEntityWithTimer(&GetEntity(), 0.0f);
                    break;

                case PowerupPickupType::kIncreaseAimBox:
                    // handle executing the powerup
                    pickup_service_->AddEntityWithPowerup(&GetEntity(),
                                                          "IncreaseAimBox");
                    pickup_service_->AddEntityWithTimer(&GetEntity(), 0.0f);
                    break;

                case PowerupPickupType::kKillAbilities:
                    // handle executing the powerup
                    pickup_service_->AddEntityWithPowerup(&GetEntity(),
                                                          "KillAbilities");
                    pickup_service_->AddEntityWithTimer(&GetEntity(), 0.0f);
                    break;
            }
        }
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
    command_.rear_brake = GetRearBrake();
    vehicle_->SetCommand(command_);

    // key to manually respawn the car
    if (input_service_->IsKeyDown(GLFW_KEY_F) ||
        input_service_->IsGamepadButtonDown(kGamepadId,
                                            GLFW_GAMEPAD_BUTTON_GUIDE))
    {
        // start the timer.
        respawn_timer_ += delta_time.GetSeconds();

        // as the button was held down for 3 seconds, we respawn the car at the
        // previous checkpoint location.
        if (respawn_timer_ >= kRespawnSeconds)
        {
            // reset the transform of the car of that of the last checkpoint it
            // crossed over. using game service to find out the information
            // about this car and getting the last crossed checkpoint.
            respawn_timer_ = 0.0f;
            vehicle_->Respawn();
        }
    }
    else if (!input_service_->IsKeyDown(GLFW_KEY_F) ||
             !input_service_->IsGamepadButtonDown(kGamepadId,
                                                  GLFW_GAMEPAD_BUTTON_GUIDE))
    {
        // reset the timer
        respawn_timer_ = 0.f;
    }
    // to respawn the car
}

float PlayerController::GetSteerDirection()
{
    if (input_service_->IsKeyDown(GLFW_KEY_A))
    {
        if (pickup_service_->IsVehicleDisableHandling(&GetEntity()))
            return 1.0f * kHandlingMultiplier;
        else
        {
            return 1.0f;
        }
    }
    else if (input_service_->IsKeyDown(GLFW_KEY_D))
    {
        if (pickup_service_->IsVehicleDisableHandling(&GetEntity()))
            return -1.0f * kHandlingMultiplier;
        else
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
    float gamepad_trigger_left = 0.0f;
    float gamepad_trigger_right = 0.0f;

    if (input_service_->IsGamepadActive(kGamepadId))
    {
        gamepad_trigger_left =
            math::Map(input_service_->GetGamepadAxis(
                          kGamepadId, GLFW_GAMEPAD_AXIS_LEFT_TRIGGER),
                      -1.0f, 1.0f, 0.0f, 1.0f);
        gamepad_trigger_right =
            math::Map(input_service_->GetGamepadAxis(
                          kGamepadId, GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER),
                      -1.0f, 1.0f, 0.0f, 1.0f);
    }

    if (forward_gear_)
    {
        if (input_service_->IsKeyDown(GLFW_KEY_W))
        {
            if (pickup_service_->IsVehicleSlowDown(&GetEntity()))
                return 1.0f * kSpeedMultiplier;
            else
                return 1.0f;
        }
        else if (input_service_->IsKeyDown(GLFW_KEY_S))
        {
            return 0.0f;
        }
        return gamepad_trigger_right;
    }
    else
    {
        if (input_service_->IsKeyDown(GLFW_KEY_W))
        {
            return 0.0f;
        }
        else if (input_service_->IsKeyDown(GLFW_KEY_S))
        {
            if (pickup_service_->IsVehicleSlowDown(&GetEntity()))
                return 1.0f * kSpeedMultiplier;
            else
                return 1.0f;
        }
        return gamepad_trigger_left;
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
            vehicle_->SetGear(VehicleGear::kForward);
        }
        else
        {
            vehicle_->SetGear(VehicleGear::kReverse);
        }
    }
}

float PlayerController::GetFrontBrake()
{
    float gamepad_trigger_left = 0.0f;
    float gamepad_trigger_right = 0.0f;

    if (input_service_->IsGamepadActive(kGamepadId))
    {
        gamepad_trigger_left =
            math::Map(input_service_->GetGamepadAxis(
                          kGamepadId, GLFW_GAMEPAD_AXIS_LEFT_TRIGGER),
                      -1.0f, 1.0f, 0.0f, 1.0f);
        gamepad_trigger_right =
            math::Map(input_service_->GetGamepadAxis(
                          kGamepadId, GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER),
                      -1.0f, 1.0f, 0.0f, 1.0f);
    }

    if (input_service_->IsKeyDown(GLFW_KEY_S) && forward_gear_)
    {
        return 1.0f;
    }
    else if (input_service_->IsKeyDown(GLFW_KEY_W) && !forward_gear_)
    {
        return 1.0f;
    }

    if (input_service_->IsGamepadActive(kGamepadId))
    {
        if (forward_gear_)
        {
            return glm::max(gamepad_trigger_left, kDefaultBrake);
        }
        else
        {
            return glm::max(gamepad_trigger_right, kDefaultBrake);
        }
    }

    return 0.0f;
}

float PlayerController::GetRearBrake()
{
    if (input_service_->IsKeyDown(GLFW_KEY_E) ||
        input_service_->IsGamepadButtonDown(kGamepadId, GLFW_GAMEPAD_BUTTON_A))
    {
        return 1.0f;
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
    float gamepad_trigger_left = 0.0f;
    float gamepad_trigger_right = 0.0f;

    if (input_service_->IsGamepadActive(kGamepadId))
    {
        gamepad_trigger_left =
            math::Map(input_service_->GetGamepadAxis(
                          kGamepadId, GLFW_GAMEPAD_AXIS_LEFT_TRIGGER),
                      -1.0f, 1.0f, 0.0f, 1.0f);
        gamepad_trigger_right =
            math::Map(input_service_->GetGamepadAxis(
                          kGamepadId, GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER),
                      -1.0f, 1.0f, 0.0f, 1.0f);
    }

    if (forward_gear_)
    {
        return input_service_->IsKeyPressed(GLFW_KEY_S) ||
               (gamepad_trigger_left > 0.1f);
    }
    else
    {
        return input_service_->IsKeyPressed(GLFW_KEY_W) ||
               (gamepad_trigger_right > 0.1f);
    }
}