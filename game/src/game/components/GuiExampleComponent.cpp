#include "game/components/GuiExampleComponent.h"

#include <imgui.h>

#include "engine/core/debug/Log.h"

using std::string_view;

GuiExampleComponent::GuiExampleComponent(GuiService& gui_service)
{
    Log::info("GuiExampleComponent - ctor");
    events_.Subscribe(gui_service.on_gui_event_,
                      std::bind(&GuiExampleComponent::OnGui, this));
}

void GuiExampleComponent::Init()
{
    Log::info("GuiExampleComponent - Init");
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
