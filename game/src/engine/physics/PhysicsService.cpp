#include "engine/physics/PhysicsService.h"

#include "engine/core/debug/Log.h"
#include "engine/service/ServiceProvider.h"

using std::string_view;

void PhysicsService::OnInit()
{
}

void PhysicsService::OnStart(ServiceProvider& service_provider)
{
}

void PhysicsService::OnUpdate()
{
}

void PhysicsService::OnCleanup()
{
}

string_view PhysicsService::GetName() const
{
    return "PhysicsService";
}