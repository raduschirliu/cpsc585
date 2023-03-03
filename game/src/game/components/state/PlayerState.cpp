#include "PlayerState.h"

#include <iostream>

void PlayerState::OnInit(const ServiceProvider& service_provider)
{
    GetEventBus().Subscribe<OnUpdateEvent>(this);
    game_state_service_ = &service_provider.GetService<GameStateService>();

    game_state_service_->AddPlayerDetails(GetEntity().GetId(), player_state_);
    // so that next time we do not enter this loop and redundantly add
    // more in the player details
    game_state_assigned_ = true;
}

void PlayerState::OnStart()
{
}

void PlayerState::OnUpdate(const Timestep& delta_time)
{
    // time elapsed in the car since game started
    player_state_.time_elapsed += delta_time.GetSeconds();
}

std::string_view PlayerState::GetName() const
{
    return "Player State";
}

// getters
float PlayerState::GetSpeedMultiplier()
{
    return player_state_.speed_multiplier;
}

int PlayerState::GetKills()
{
    return player_state_.number_kills;
}

int PlayerState::GetDeaths()
{
    return player_state_.number_deaths;
}

int PlayerState::GetLapsCompleted()
{
    return player_state_.laps_completed;
}

int PlayerState::GetCurrentLap()
{
    return player_state_.current_lap;
}

Entity* PlayerState::GetNemesis()
{
    return player_state_.nemesis;
}

Entity* PlayerState::GetBullied()
{
    return player_state_.bullied;
}

double PlayerState::GetTimeElapsed()
{
    return player_state_.time_elapsed;
}

PowerupPickupType PlayerState::GetCurrentPowerup()
{
    return player_state_.current_powerup;
}

PlayerStateData* PlayerState::GetStateData()
{
    return &player_state_;
}

float PlayerState::SetSpeedMultiplier(float value)
{
    return player_state_.speed_multiplier = value;
}