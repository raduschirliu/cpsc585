#include "game/GameService.h"

#include "engine/input/InputService.h"
#include "engine/scene/Entity.h"
#include "engine/service/ServiceProvider.h"

std::string_view GameService::GetName() const
{
    return "GameService";
}

void GameService::OnInit()
{
}

void GameService::OnStart(ServiceProvider& service_provider)
{
    GameData game_data;
    game_data.number_of_players = 4;
}