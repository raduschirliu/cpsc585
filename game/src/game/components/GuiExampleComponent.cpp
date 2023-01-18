#include "game/components/GuiExampleComponent.h"

#include <imgui.h>

#include "engine/core/debug/Log.h"

using std::string_view;

GuiExampleComponent::GuiExampleComponent()
{
    Log::info("GuiExampleComponent - ctor");
}

void GuiExampleComponent::Init(ComponentInitializer& initializer)
{
    Log::info("GuiExampleComponent - Init");

    initializer.event_bus.Subscribe<OnGuiEvent>(this);
}

string_view GuiExampleComponent::GetName() const
{
    return "GuiExampleComponent";
}

void GuiExampleComponent::OnGui()
{
    if (ImGui::Begin("GuiExampleComponent"))
    {
        ImGui::Text("hello world");
        ImGui::Button("useless button");
        ImGui::End();
    }
}
