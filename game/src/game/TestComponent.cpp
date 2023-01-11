#include "game/TestComponent.h"

#include "engine/core/debug/Log.h"

using std::string_view;

void TestComponent::Init()
{
    Log::info("TestComponent - Init");
}

string_view TestComponent::GetName() const
{
    return "TestComponent";
}
