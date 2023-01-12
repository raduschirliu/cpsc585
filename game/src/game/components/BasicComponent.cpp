#include "game/components/BasicComponent.h"

#include "engine/core/debug/Log.h"

using std::string_view;

BasicComponent::BasicComponent()
{
    Log::info("BasicComponent - ctor");
}

void BasicComponent::Init()
{
    Log::info("BasicComponent - Init");
}

string_view BasicComponent::GetName() const
{
    return "BasicComponent";
}
