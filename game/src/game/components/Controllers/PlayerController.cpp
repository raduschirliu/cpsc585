#include "PlayerController.h"

#include <imgui.h>

#include "engine/App.h"
#include "engine/core/Colors.h"
#include "engine/core/debug/Log.h"
#include "engine/core/math/Common.h"
#include "engine/input/InputService.h"
#include "engine/physics/PhysicsService.h"
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
static constexpr float kSpeedMultiplier = 1.0f;
static constexpr float kHanldingMultiplier = 1.0f;

void PlayerController::OnInit(const ServiceProvider& service_provider)
{
    input_service_ = &service_provider.GetService<InputService>();
    game_state_service_ = &service_provider.GetService<GameStateService>();

    transform_ = &GetEntity().GetComponent<Transform>();
    player_data_ = &GetEntity().GetComponent<PlayerState>();
    vehicle_ = &GetEntity().GetComponent<VehicleComponent>();
    shooter_ = &GetEntity().GetComponent<Shooter>();

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
    if (shoot_cooldown_ > 0.0f)
    {
        shoot_cooldown_ -= static_cast<float>(delta_time.GetSeconds());
        return;
    }

    if (input_service_->IsKeyPressed(GLFW_KEY_R) ||
        input_service_->IsGamepadButtonPressed(kGamepadId,
                                               GLFW_GAMEPAD_BUTTON_B))
    {
        shooter_->Shoot();
        shoot_cooldown_ = shooter_->GetCooldownTime();
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
            // debug::LogDebug("executing the powerup");
            // power executed, so add it to the map in game service.
            game_state_service_->AddPlayerPowerup(
                GetEntity().GetId(), player_data_->GetCurrentPowerup());
        }
    }

    // this means that the everyone slower pickup is active right now.
    int id = game_state_service_->GetEveryoneSlowerSpeedMultiplier();
    if (id != -1)
    {
        // now except for the entity who launched it, all the entities should
        // slow down.
        int entity_id = GetEntity().GetId();
        if (entity_id != id)
        {
            // if any AI picked up the powerup then the player's speed should be
            // reduced.
            speed_multiplier_ = 0.2f;
            vehicle_->SetMaxAchievableVelocity(40.f);
        }
        else
        {
            // this is the entity which started the powerup, so do nothing.
        }
    }
    else
    {
        speed_multiplier_ = kSpeedMultiplier;
        vehicle_->SetMaxAchievableVelocity(100.f);
    }
    id = game_state_service_->GetDisableHandlingMultiplier();
    if (id != -1)
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
        }
    }
    else
    {
        // this is the entity which started the powerup, so do nothing.
        handling_multiplier_ = kHanldingMultiplier;
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

    // key to respawn the car using.
    if (input_service_->IsKeyDown(GLFW_KEY_F) ||
        input_service_->IsGamepadButtonDown(kGamepadId,
                                            GLFW_GAMEPAD_BUTTON_GUIDE))
    {
        // start the timer.
        button_down_respawn_timer += delta_time.GetSeconds();

        // as the button was held down for 3 seconds, we respawn the car at the
        // previous checkpoint location.
        if (button_down_respawn_timer >= kRespawnSeconds)
        {
            // reset the transform of the car of that of the last checkpoint it
            // crossed over. using game service to find out the information
            // about this car and getting the last crossed checkpoint.
            button_down_respawn_timer = 0.0f;
            RespawnCar();
        }
    }
    else if (!input_service_->IsKeyDown(GLFW_KEY_F) ||
             !input_service_->IsGamepadButtonDown(kGamepadId,
                                                  GLFW_GAMEPAD_BUTTON_GUIDE))
    {
        // reset the timer
        button_down_respawn_timer = 0.f;
    }
    // to respawn the car
}

void PlayerController::FixRespawnOrientation(
    const glm::vec3& next_checkpoint_location,
    const glm::vec3& checkpoint_location)
{
    auto current_orientation = transform_->GetOrientation();

    glm::vec3 direction =
        glm::normalize(next_checkpoint_location - checkpoint_location);
    glm::vec3 forward =
        transform_->GetForwardDirection();  // assume car is initially oriented
                                            // along the negative z-axis
    glm::vec3 axis = glm::normalize(glm::cross(forward, direction));
    float angle = glm::acos(glm::dot(forward, direction));
    transform_->SetOrientation(glm::angleAxis(angle, axis) *
                               current_orientation);
}

void PlayerController::RespawnCar()
{
    // need this so that we can change the transform of this car to be that.
    // this will act as out parameter and have information in it when the
    // function returns.
    glm::vec3 checkpoint_location;
    glm::vec3 next_checkpoint_location;

    int current_checkpoint = game_state_service_->GetCurrentCheckpoint(
        this->GetEntity().GetId(), checkpoint_location,
        next_checkpoint_location);

    if (current_checkpoint == -1)
        return;

    transform_->SetPosition(checkpoint_location);
    FixRespawnOrientation(next_checkpoint_location, checkpoint_location);

    game_state_service_->AddRespawnPlayers(this->GetEntity().GetId());
}

float PlayerController::GetSteerDirection()
{
    if (input_service_->IsKeyDown(GLFW_KEY_A))
    {
        return 1.0f * handling_multiplier_;
    }
    else if (input_service_->IsKeyDown(GLFW_KEY_D))
    {
        return -1.0f * handling_multiplier_;
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
            return 1.0f * speed_multiplier_;
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