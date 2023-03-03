#pragma once
#include <memory>

#include "../Pickups/PickupType.h"

#include "../../../engine/scene/Entity.h"

struct PlayerStateData
{
    float speed_multiplier;
    int number_kills;
    int number_deaths;
    int laps_completed;
    int current_lap;
    Entity* nemesis;
    Entity* bullied;
    double time_elapsed;
    PowerupPickupType current_powerup;  // will be decided by the enum defined for every
                          // powerup.
    double accuracy;

    PlayerStateData()
    {
        speed_multiplier = 1.f;
        number_deaths = 0;
        number_kills = 0;
        laps_completed = 0;
        current_lap = 0;
        current_powerup = PowerupPickupType::kDefaultPowerup;
        time_elapsed = 0.f;
        nemesis = nullptr;
        bullied = nullptr;
        accuracy = 0.f;
    }
};