#include "game/components/FinishLineComponent.h"

#include "engine/core/debug/Log.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"

void FinishLineComponent::OnInit(const ServiceProvider& service_provider)
{
    game_service_ = &service_provider.GetService<GameStateService>();
}

void FinishLineComponent::OnTriggerEnter(const OnTriggerEvent& data)
{
    game_service_->PlayerCompletedLap(*data.other);
}

std::string_view FinishLineComponent::GetName() const
{
    return "FinishLine";
}
