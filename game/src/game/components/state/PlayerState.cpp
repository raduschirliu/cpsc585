#include "PlayerState.h"

#include <iostream>

#include "engine/core/debug/Log.h"
#include "engine/physics/PhysicsService.h"
#include "game/services/GameStateService.h"

static constexpr float kDeathCooldownSeconds = 5.0f;

void PlayerState::OnInit(const ServiceProvider& service_provider)
{
    // service and component dependencies
    physics_service_ = &service_provider.GetService<PhysicsService>();
    game_state_service_ = &service_provider.GetService<GameStateService>();

    vehicle_ = &GetEntity().GetComponent<VehicleComponent>();
    audio_emitter_ = &GetEntity().GetComponent<AudioEmitter>();

    // reset cooldown
    death_cooldown_ = 0.0f;

    // load sounds
    audio_emitter_->AddSource("pickup_get_01.ogg");
    audio_emitter_->SetGain("pickup_get_01.ogg", 0.3f);
    audio_emitter_->AddSource("pickup_get_02.ogg");
    audio_emitter_->SetGain("pickup_get_02.ogg", 0.3f);
    audio_emitter_->AddSource("kart_hit_01.ogg");
    audio_emitter_->SetGain("kart_hit_01.ogg", 0.3f);
    audio_emitter_->AddSource("player_die_01.ogg");
    audio_emitter_->SetGain("player_die_01.ogg", 2.0f);
    audio_emitter_->AddSource("wrong_buzz.ogg");
    audio_emitter_->SetGain("wrong_buzz.ogg", 1.0f);

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
        if (death_cooldown_ <= 0)
        {
            player_state_.is_dead = false;
            player_state_.health = 100.0f;
            vehicle_->Respawn();
            debug::LogDebug("Entity {} recovered!", GetEntity().GetId());
        }
        else
        {
            // player can't move while deadge
            auto& vehicle_state = vehicle_->GetVehicle().mBaseState;
            auto& wheel_states = vehicle_state.wheelRigidBody1dStates;
            float& wheel_speed = wheel_states->rotationSpeed = 0.0f;

            float delta_time_seconds =
                static_cast<float>(delta_time.GetSeconds());
            death_cooldown_ -= delta_time_seconds;
        }
    }
    else
    {
        // player has deadge
        if (player_state_.health <= 0.0f)
        {
            death_cooldown_ = kDeathCooldownSeconds;
            player_state_.is_dead = true;
            player_state_.number_deaths++;
            audio_emitter_->PlaySource("player_die_01.ogg");
            debug::LogDebug("Entity {} has died!", GetEntity().GetId());
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

float PlayerState::GetDeathCooldown() const
{
    return death_cooldown_;
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
    if (player_state_.health >= health && !physics_service_->GetPaused())
    {
        player_state_.health -= health;
        audio_emitter_->PlaySource("kart_hit_01.ogg");
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
    if (physics_service_->GetPaused())
    {
        return;
    }

    player_state_.current_powerup = type;

    if (type != PowerupPickupType::kDefaultPowerup)
    {
        audio_emitter_->PlaySource("pickup_get_01.ogg");
    }
}

void PlayerState::SetCurrentAmmoType(AmmoPickupType type)
{
    player_state_.current_ammo_type = type;

    if (type != AmmoPickupType::kDefaultAmmo)
    {
        audio_emitter_->PlaySource("pickup_get_02.ogg");
    }
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

float PlayerState::GetHandlingMultiplier()
{
    return player_state_.handling_multiplier;
}

void PlayerState::SetHandlingMultiplier(float multiplier)
{
    player_state_.handling_multiplier = multiplier;
}

void PlayerState::SetMaxCarSpeed(float max_speed)
{
    player_state_.max_car_speed = max_speed;
}

float PlayerState::GetMaxCarSpeed()
{
    return player_state_.max_car_speed;
}