#include "game/components/race/FinishLine.h"

#include "engine/core/debug/Log.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"

void FinishLine::OnInit(const ServiceProvider& service_provider)
{
    game_service_ = &service_provider.GetService<GameStateService>();
}

void FinishLine::OnTriggerEnter(const OnTriggerEvent& data)
{
    game_service_->PlayerFinished(*data.other);
}

std::string_view FinishLine::GetName() const
{
    return "FinishLine";
}
