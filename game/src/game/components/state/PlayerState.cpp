#include "PlayerState.h"

#include <iostream>

static constexpr float kMaxDeathCooldown = 5.0f;

void PlayerState::OnInit(const ServiceProvider& service_provider)
{
    game_state_service_ = &service_provider.GetService<GameStateService>();
    death_cooldown = 0.f;

    GetEventBus().Subscribe<OnUpdateEvent>(this);

    player_state_.Reset();
}

void PlayerState::OnStart()
{
}

void PlayerState::OnUpdate(const Timestep& delta_time)
{
    CheckDead(delta_time);
}

void PlayerState::CheckDead(const Timestep& delta_time)
{
    if (player_state_.is_dead)  // lol
    {
        // cooldown up, no longer dead
        if (death_cooldown <= 0)
        {
            player_state_.is_dead = false;
            player_state_.health = 100.0f;
        }
        else
        {
            float delta_time_seconds =
                static_cast<float>(delta_time.GetSeconds());
            death_cooldown -= delta_time_seconds;
        }
    }
    else
    {
        // player has deadge
        if (player_state_.health <= 0.0f)
        {
            death_cooldown = kMaxDeathCooldown;
            player_state_.is_dead = true;
            player_state_.number_deaths++;
        }
    }
}

std::string_view PlayerState::GetName() const
{
    return "PlayerState";
}

// getters
bool PlayerState::IsDead() const
{
    return player_state_.is_dead;
}

float PlayerState::GetSpeedMultiplier() const
{
    return player_state_.speed_multiplier;
}

int PlayerState::GetKills() const
{
    return player_state_.number_kills;
}

int PlayerState::GetDeaths() const
{
    return player_state_.number_deaths;
}

int PlayerState::GetLapsCompleted() const
{
    return player_state_.laps_completed;
}

Entity* PlayerState::GetNemesis()
{
    return player_state_.nemesis;
}

Entity* PlayerState::GetBullied()
{
    return player_state_.bullied;
}

PowerupPickupType PlayerState::GetCurrentPowerup() const
{
    return player_state_.current_powerup;
}

AmmoPickupType PlayerState::GetCurrentAmmoType() const
{
    return player_state_.current_ammo_type;
}

int PlayerState::GetLastCheckpoint() const
{
    return player_state_.last_checkpoint;
}

float PlayerState::GetHealth() const
{
    return player_state_.health;
}

PlayerStateData* PlayerState::GetStateData()
{
    return &player_state_;
}

// setters
void PlayerState::SetHealth(float health)
{
    player_state_.health = health;
}

void PlayerState::DecrementHealth(float health)
{
    if (player_state_.health > health)
    {
        player_state_.health -= health;
    }
}

void PlayerState::IncrementHealth(float health)
{
    if (player_state_.health < 100.0f - health)
    {
        player_state_.health += health;
    }
}

void PlayerState::SetSpeedMultiplier(float value)
{
    player_state_.speed_multiplier = value;
}

void PlayerState::SetCurrentPowerup(PowerupPickupType type)
{
    player_state_.current_powerup = type;
}

void PlayerState::SetCurrentAmmoType(AmmoPickupType type)
{
    player_state_.current_ammo_type = type;
}

void PlayerState::SetLapsCompleted(int laps)
{
    player_state_.laps_completed = laps;
}

void PlayerState::SetLastCheckpoint(int checkpoint)
{
    player_state_.last_checkpoint = checkpoint;
}

int PlayerState::GetCurrentPlace() const
{
    return player_state_.place;
}

void PlayerState::SetCurrentPlace(int place)
{
    player_state_.place = place;
}