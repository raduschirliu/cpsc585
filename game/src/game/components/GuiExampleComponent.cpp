#include "game/components/GuiExampleComponent.h"

#include <imgui.h>

#include "engine/core/debug/Log.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"

using std::string_view;

void GuiExampleComponent::OnInit(const ServiceProvider& service_provider)
{
    // Dependencies
    input_service_ = &service_provider.GetService<InputService>();
    physics_service_ = &service_provider.GetService<PhysicsService>();

    transform_ = &GetEntity().GetComponent<Transform>();

    // Events
    GetEventBus().Subscribe<OnGuiEvent>(this);
    GetEventBus().Subscribe<OnUpdateEvent>(this);
}

void GuiExampleComponent::OnUpdate(const Timestep& delta_time)
{
    if (input_service_->IsKeyPressed(GLFW_KEY_F))
    {
        Log::info("repsects = paid.");
        transform_->Translate(glm::vec3(1.0f, 0.0f, 0.0f));
    }

    if (input_service_->IsKeyDown(GLFW_KEY_X))
    {
        Log::info("shaaaaaaauun");
        transform_->Translate(glm::vec3(0.0f, 0.25f, 0.0f));
    }

    if (input_service_->IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
    {
        physics_service_->CreateRaycastFromOrigin(
            transform_->GetPosition(), transform_->GetForwardDirection());
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
        const glm::vec3& pos = transform_->GetPosition();

        ImGui::Text("Mouse pos: %d %d", mouse_pos.x, mouse_pos.y);
        ImGui::Text("Entity pos: %f %f %f", pos.x, pos.y, pos.z);

        ImGui::End();
    }
}
