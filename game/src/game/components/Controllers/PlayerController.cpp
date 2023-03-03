#include "PlayerController.h"

#include "engine/core/debug/Log.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"
#include "game/components/state/PlayerState.h"

float kSpeedMultiplier(1.f);

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
    // getting the player_data struct so that we can use it for different stuff.
    if (!player_data_)
    {
        player_data_ = &GetEntity().GetComponent<PlayerState>();
    }

    if (input_service_->IsKeyDown(GLFW_KEY_SPACE))
    {
        if (player_data_)
        {
            if (!execute_powerup_ && player_data_->GetCurrentPowerup() ==
                                         PowerupPickupType::kDefaultPowerup)
            {
                Log::debug("You currently do not have any powerup.");
            }

            // so that this is not called every frame.
            else if (!execute_powerup_)
            {
                // Now we can do the logic for different powerups
                execute_powerup_ = true;

                if (player_data_->GetCurrentPowerup() ==
                    PowerupPickupType::kEveryoneSlower)
                {
                    speed_multiplier_ = 0.2f;
                }
            }
        }
    }

    // timer_ stuff.
    if (execute_powerup_)
    {
        timer_ += delta_time.GetSeconds();
        CheckTimer(5.f, player_data_->GetCurrentPowerup());
    }

    // Control the car.
    CarController(delta_time);
}

void PlayerController::CheckTimer(double timer_limit,
                                  PowerupPickupType pickup_type)
{
    if (player_data_)
    {
        if (timer_ > timer_limit)
        {
            timer_ = 0.f;
            if (pickup_type == PowerupPickupType::kEveryoneSlower)
            {
                player_data_->SetCurrentPowerup(
                    PowerupPickupType::kDefaultPowerup);
                speed_multiplier_ = kSpeedMultiplier;
            }
            execute_powerup_ = false;
        }
    }
}

std::string_view PlayerController::GetName() const
{
    return "Player Controller";
}

void PlayerController::CarController(const Timestep& delta_time)
{
    executable_command_ =
        new Command();  // the default Command slows down the car.

    if (executable_command_)
    {
        if (input_service_->IsKeyDown(GLFW_KEY_UP) ||
            input_service_->IsKeyDown(GLFW_KEY_W))
        {
            Command temp(0.0f, 1.0f * speed_multiplier_, 0.0f, timestep_);
            *executable_command_ = temp;
        }
        if (input_service_->IsKeyDown(GLFW_KEY_LEFT) ||
            input_service_->IsKeyDown(GLFW_KEY_A))
        {
            Command temp(0.0f, 0.1f, -0.4f, timestep_);
            *executable_command_ = temp;
        }
        if (input_service_->IsKeyDown(GLFW_KEY_RIGHT) ||
            input_service_->IsKeyDown(GLFW_KEY_D))
        {
            Command temp(0.0f, 0.1f, 0.4f, timestep_);
            *executable_command_ = temp;
        }
        if (input_service_->IsKeyDown(GLFW_KEY_DOWN) ||
            input_service_->IsKeyDown(GLFW_KEY_S))
        {
            Command temp(1.0f, 0.0f, 0.0f, timestep_);
            *executable_command_ = temp;
        }

        vehicle_reference_->mCommandState.brakes[0] =
            executable_command_->brake;
        vehicle_reference_->mCommandState.nbBrakes = 1;
        vehicle_reference_->mCommandState.throttle =
            executable_command_->throttle;
        vehicle_reference_->mCommandState.steer = executable_command_->steer;
    }
    delete (executable_command_);
}