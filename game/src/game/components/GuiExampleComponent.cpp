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

    input_service_ = &service_provider.GetService<InputService>();

    GetEventBus().Subscribe<OnGuiEvent>(this);
    GetEventBus().Subscribe<OnUpdateEvent>(this);
}

void GuiExampleComponent::OnUpdate()
{
    if (input_service_->IsKeyPressed(GLFW_KEY_F))
    {
        Log::info("repsects = paid.");
    }

    if (input_service_->IsKeyDown(GLFW_KEY_X))
    {
        Log::info("shaaaaaaauun");
    }
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

        glm::ivec2 mouse_pos = input_service_->GetMousePos();
        ImGui::Text("Mouse pos: %d %d", mouse_pos.x, mouse_pos.y);

        ImGui::End();
    }
}
