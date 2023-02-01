#include "game/components/BasicComponent.h"

#include "engine/core/debug/Log.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"

using std::string_view;

void BasicComponent::OnInit(const ServiceProvider& service_provider)
{
    Log::info("BasicComponent - Init");

    GetEventBus().Subscribe<OnUpdateEvent>(this);
}

void BasicComponent::OnUpdate()
{
}

string_view BasicComponent::GetName() const
{
    return "BasicComponent";
}
