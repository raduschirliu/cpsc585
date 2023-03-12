#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "engine/core/Colors.h"

struct CheckpointRecord
{
    int index;
    Entity* entity;
    glm::vec3 position;
    float distance_to_next;
};

struct PlayerSpawnConfig
{
    glm::vec3 position;
    glm::vec3 orientation_euler_degrees;
    Color3 color;
};

struct TrackConfig
{
    std::vector<PlayerSpawnConfig> player_spawns;
    std::vector<CheckpointRecord> checkpoints;

    void Reset()
    {
        player_spawns.clear();
        checkpoints.clear();
    }
};

struct RaceConfig
{
    uint32_t num_human_players;
    uint32_t num_ai_players;
    uint32_t num_laps;

    RaceConfig() : num_human_players(0), num_ai_players(0), num_laps(0)
    {
    }
};
