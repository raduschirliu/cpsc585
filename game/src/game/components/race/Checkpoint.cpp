#include "game/components/race/Checkpoint.h"

#include "engine/core/debug/Log.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"

void Checkpoint::OnInit(const ServiceProvider& service_provider)
{
    game_service_ = &service_provider.GetService<GameStateService>();
}

void Checkpoint::OnTriggerEnter(const OnTriggerEvent& data)
{
    game_service_->PlayerFinished(*data.other);
}

std::string_view Checkpoint::GetName() const
{
    return "Checkpoint";
}
