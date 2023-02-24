#pragma once
#include "../../../engine/scene/Entity.h"

struct PlayerStateStruct
{
    float speed;
    int number_kills;
    int number_deaths;
    int laps_completed;
    int current_lap;
    Entity* nemesis;
    Entity* bullied;
    float time_elapsed;
    int current_powerup;  // will be decided by the enum defined for every
                          // powerup.
    float accuracy;

    PlayerStateStruct()
    {
        speed = 0.f;
        number_deaths = 0;
        number_kills = 0;
        laps_completed = 0;
        current_lap = 0;
        nemesis = nullptr;
        bullied = nullptr;
        time_elapsed = 0.f;
        current_powerup = 0;
    }
};