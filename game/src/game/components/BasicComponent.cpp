#include "game/components/BasicComponent.h"

#include "engine/core/debug/Log.h"

using std::string_view;

void BasicComponent::Init(const ServiceProvider& service_provider)
{
    Log::info("BasicComponent - Init");
}

string_view BasicComponent::GetName() const
{
    return "BasicComponent";
}
