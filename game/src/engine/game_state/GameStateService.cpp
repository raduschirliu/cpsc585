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
}

void GameStateService::OnCleanup()
{
}

std::string_view GameStateService::GetName() const
{
    return "Game State Service";
}