#include "engine/physics/PhysicsService.h"

#include "engine/core/debug/Log.h"

using std::string_view;

void PhysicsService::Init()
{
    Log::info("PhysicsService - Initializing");
}

void PhysicsService::Start()
{
}

void PhysicsService::Update()
{
}

void PhysicsService::Cleanup()
{
    Log::info("PhysicsService - Cleaning up");
}

string_view PhysicsService::GetName() const
{
    return "PhysicsService";
}