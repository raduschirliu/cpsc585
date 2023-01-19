#include "game/components/GuiExampleComponent.h"

#include <imgui.h>

#include "engine/core/debug/Log.h"

using std::string_view;

void GuiExampleComponent::OnInit(const ServiceProvider& service_provider)
{
    Log::info("GuiExampleComponent - Init");

    RenderService& render_service =
        service_provider.GetService<RenderService>();
    render_service.SayHi();

    GetEventBus().Subscribe<OnGuiEvent>(this);
}

void GuiExampleComponent::OnUpdate()
{
}

string_view GuiExampleComponent::GetName() const
{
    return "GuiExampleComponent";
}

void GuiExampleComponent::OnGui()
{
    ImGui::ShowDemoWindow(nullptr);

    if (ImGui::Begin("GuiExampleComponent"))
    {
        ImGui::Text("hello world");
        ImGui::Button("useless button");
        ImGui::End();
    }
}
