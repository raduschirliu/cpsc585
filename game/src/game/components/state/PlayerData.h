#pragma once

#include "../Pickups/PickupType.h"

class Entity;

struct PlayerStateData
{
    float speed_multiplier;
    int number_kills;
    int number_deaths;
    int laps_completed;
    int last_checkpoint;
    Entity* nemesis;
    Entity* bullied;
    PowerupPickupType current_powerup;  // will be decided by the enum defined
                                        // for every powerup.
    double accuracy;

    PlayerStateData()
    {
        Reset();
    }

    void Reset()
    {
        speed_multiplier = 1.0f;
        number_deaths = 0;
        number_kills = 0;
        laps_completed = 0;
        last_checkpoint = 0;
        current_powerup = PowerupPickupType::kDefaultPowerup;
        nemesis = nullptr;
        bullied = nullptr;
        accuracy = 0.0f;
    }
};