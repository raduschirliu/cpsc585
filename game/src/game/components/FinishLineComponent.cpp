#include "game/components/FinishLineComponent.h"

#include "engine/core/debug/Log.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"

void FinishLineComponent::OnInit(const ServiceProvider& service_provider)
{
}

void FinishLineComponent::OnTriggerEnter(const OnTriggerEvent& data)
{
    Log::debug("finish line hit by {}", data.other->GetName());
}

std::string_view FinishLineComponent::GetName() const
{
    return "FinishLine";
}
