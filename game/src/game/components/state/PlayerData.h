#pragma once

#include "../Pickups/PickupType.h"

class Entity;

struct PlayerStateData
{
    float health;
    bool is_dead;
    float speed_multiplier;
    float handling_multiplier;
    float max_car_speed;
    int number_kills;
    int number_deaths;
    int laps_completed;
    int last_checkpoint;
    int place;
    Entity* nemesis;
    Entity* bullied;
    PowerupPickupType current_powerup;  // will be decided by the enum defined
                                        // for every powerup.
    AmmoPickupType current_ammo_type;
    double accuracy;

    PlayerStateData()
    {
        Reset();
    }

    void Reset()
    {
        handling_multiplier = 1.0f;
        health = 100.0f;
        speed_multiplier = 1.0f;
        number_deaths = 0;
        number_kills = 0;
        laps_completed = 0;
        last_checkpoint = 0;
        current_powerup = PowerupPickupType::kDefaultPowerup;
        place = 0;
        nemesis = nullptr;
        bullied = nullptr;
        accuracy = 0.0f;
        max_car_speed = 100.f;
    }
};