#include "game/components/ComplexComponent.h"

#include "engine/core/debug/Log.h"

using std::string_view;

ComplexComponent::ComplexComponent()
{
    Log::info("ComplexComponent - ctor");
}

void ComplexComponent::Init(ComponentInitializer& initializer)
{
    Log::info("ComplexComponent - Init");

    RenderService& render_service =
        initializer.service_provider.GetService<RenderService>();
    render_service.SayHi();
}

string_view ComplexComponent::GetName() const
{
    return "ComplexComponent";
}
