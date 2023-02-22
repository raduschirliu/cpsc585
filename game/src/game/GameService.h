#pragma once
#include <ctype.h>

#include <optional>
#include <vector>

#include "engine/input/InputService.h"
#include "engine/service/Service.h"

struct GameData
{
    int number_of_players;
    int players_finished;

    float lap_time;
    float time_elapsed;
    float fastest_lap;

    int player_1_kills;
    int player_2_kills;
    int player_3_kills;
    int player_4_kills;

    int player_1_deaths;
    int player_2_deaths;
    int player_3_deaths;
    int player_4_deaths;
};

class GameService final : public Service
{
    // From Service
    void OnInit() override;
    void OnStart(ServiceProvider& service_provider) override;
    void OnUpdate() override;
    void OnCleanup() override;
    std::string_view GetName() const override;
};