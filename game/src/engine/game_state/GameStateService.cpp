#include "GameStateService.h"

#include "engine/core/debug/Log.h"
#include "engine/scene/OnUpdateEvent.h"

GameStateService::GameStateService()
{
}

void GameStateService::OnInit()
{
    Log::debug("Game State Service working");
}

void GameStateService::OnStart(ServiceProvider& service_provider)
{
    // GetEventBus().Subscribe<OnGuiEvent>(this);
}

void GameStateService::OnUpdate()
{
    Log::debug("{}", player_powers_.size());
}

void GameStateService::OnCleanup()
{
}

std::string_view GameStateService::GetName() const
{
    return "Game State Service";
}

/**
 * @brief Makes a vector of powerups currently active by all the AIs and Player
 *
 * @return std::vector<PowerupPickupType>
 */
std::vector<PowerupPickupType> GameStateService::PowerupsActive()
{
    std::vector<PowerupPickupType> powerups;
    for (auto& p : player_powers_)
    {
        powerups.push_back(p.second);
    }
    return powerups;
}

void GameStateService::AddPlayerDetails(uint32_t id, PlayerStateData details)
{
    player_details_.insert_or_assign(id, details);
}

void GameStateService::AddPlayerPowerup(uint32_t id, PowerupPickupType power)
{
    player_powers_.insert_or_assign(id, power);
}

void GameStateService::RemovePlayerPowerup(uint32_t id)
{
    player_powers_.insert_or_assign(id, PowerupPickupType::kDefaultPowerup);
}