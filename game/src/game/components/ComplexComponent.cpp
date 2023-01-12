#include "game/components/ComplexComponent.h"

#include "engine/core/debug/Log.h"

using std::string_view;

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
    return "ComplexComponent";
}
