#pragma once

#include <string>

#include "../Pickups/PickupType.h"
class Entity;

struct PlayerStateData
{
    float health;
    bool is_dead;
    float speed_multiplier;
    float handling_multiplier;
    float max_car_speed;
    float current_car_speed;
    int number_kills;
    int number_deaths;
    int laps_completed;
    int last_checkpoint;
    int place;
    std::string player_name;
    std::string player_who_shot_me;
    Entity* nemesis;
    Entity* bullied;
    PowerupPickupType current_powerup;  // will be decided by the enum defined
                                        // for every powerup.
    AmmoPickupType current_ammo_type;
    double accuracy;
    bool is_accelerating;

    PlayerStateData()
    {
        Reset();
    }

    void Reset()
    {
        is_accelerating = false;
        player_name = "";
        player_who_shot_me = "";
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
        current_car_speed = 0.f;

    }
};