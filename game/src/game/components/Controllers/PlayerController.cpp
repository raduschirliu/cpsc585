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
    game_state_service_ = &service_provider.GetService<GameStateService>();
    transform_ = &GetEntity().GetComponent<Transform>();

    GetEventBus().Subscribe<OnUpdateEvent>(this);
}

void PlayerController::OnUpdate(const Timestep& delta_time)
{
    // Log::debug("{}", game_state_service_->GetActivePowerups().size());
    //  getting the player_data struct so that we can use it for different
    //  stuff.

    // Log::debug("Player ID: {} ; speed: {}", GetEntity().GetName(),
    // speed_multiplier_);

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

                // power executed, so add it to the map in game service.
                game_state_service_->AddPlayerPowerup(
                    GetEntity().GetId(), player_data_->GetCurrentPowerup());
            }
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

    // // timer_ stuff.
    // if (execute_powerup_)
    // {
    //     timer_ += delta_time.GetSeconds();
    //     CheckTimer(5.f, player_data_->GetCurrentPowerup());
    // }

    // Control the car.
    CarController(delta_time);
}

// void PlayerController::CheckTimer(double timer_limit,
//                                   PowerupPickupType pickup_type)
// {
//     if (player_data_)
//     {
//         if (timer_ > timer_limit)
//         {
//             timer_ = 0.f;
//             if (pickup_type == PowerupPickupType::kEveryoneSlower)
//             {
//                 game_state_service_->RemovePlayerPowerup(GetEntity().GetId());
//                 player_data_->SetCurrentPowerup(
//                     PowerupPickupType::kDefaultPowerup);
//                 speed_multiplier_ = kSpeedMultiplier;
//                 // as this is done now.
//                 game_state_service_->RemoveEveryoneSlowerSpeedMultiplier();
//             }
//             execute_powerup_ = false;
//         }
//     }
// }

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