#include "game/components/BasicComponent.h"

#include "engine/core/debug/Log.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"

using std::string_view;

void BasicComponent::OnInit(const ServiceProvider& service_provider)
{
    GetEventBus().Subscribe<OnUpdateEvent>(this);
}

void BasicComponent::OnUpdate(const Timestep& delta_time)
{
}

string_view BasicComponent::GetName() const
{
    return "BasicComponent";
}
