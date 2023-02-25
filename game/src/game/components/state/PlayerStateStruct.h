#pragma once
#include <memory>

#include "../../../engine/scene/Entity.h"

struct PlayerStateStruct
{
    std::shared_ptr<double> speed;
    int number_kills;
    int number_deaths;
    int laps_completed;
    int current_lap;
    Entity* nemesis;
    Entity* bullied;
    double time_elapsed;
    int current_powerup;  // will be decided by the enum defined for every
                          // powerup.
    double accuracy;

    PlayerStateStruct()
    {
        speed = nullptr;
        number_deaths = 0;
        number_kills = 0;
        laps_completed = 0;
        current_lap = 0;
        current_powerup = 0;
        time_elapsed = 0.f;
        nemesis = nullptr;
        bullied = nullptr;
        accuracy = 0.f;
    }
};