#include "game/TestComponent.h"

#include "engine/core/debug/Log.h"

using std::string_view;

TestComponent::TestComponent()
{
    Log::info("TestComponent - ctor");
}

void TestComponent::Init()
{
    Log::info("TestComponent - Init");
}

string_view TestComponent::GetName() const
{
    return "TestComponent";
}

ComplexComponent::ComplexComponent(RenderService& render_service)
{
    Log::info("ComplexComponent - ctor");
    render_service.SayHi();
}

void ComplexComponent::Init()
{
    Log::info("ComplexComponent - Init");
}

string_view ComplexComponent::GetName() const
{
    return "TestComponent";
}

