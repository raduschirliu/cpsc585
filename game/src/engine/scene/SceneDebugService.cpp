#include "engine/scene/SceneDebugService.h"

#include <imgui.h>

#include <string>

#include "engine/App.h"
#include "engine/core/debug/Log.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/service/ServiceProvider.h"

using std::string_view;

static constexpr float kComponentGuiIndent = 12.5f;

void SceneDebugService::OnInit()
{
}

void SceneDebugService::OnStart(ServiceProvider& service_provider)
{
    input_service_ = &service_provider.GetService<InputService>();

    GetEventBus().Subscribe<OnGuiEvent>(this);
}

void SceneDebugService::OnUpdate()
{
    if (input_service_->IsKeyPressed(GLFW_KEY_F1))
    {
        active_scene_ = &GetApp().GetSceneList().GetActiveScene();
        show_menu_ = !show_menu_;
    }
}

void SceneDebugService::OnCleanup()
{
}

string_view SceneDebugService::GetName() const
{
    return "SceneDebugService";
}

void SceneDebugService::OnGui()
{
    if (!show_menu_)
    {
        return;
    }

    if (!ImGui::Begin("Scene", &show_menu_))
    {
        ImGui::End();
        return;
    }

    ImGui::Text("Active scene: %s", active_scene_->GetName().c_str());

    if (ImGui::CollapsingHeader("Entities"))
    {
        DrawEntityList();
    }

    ImGui::End();
}

void SceneDebugService::DrawEntityList()
{
    for (auto& entity : active_scene_->GetEntities())
    {
        const uint32_t& entity_id = entity->GetId();

        if (ImGui::TreeNode(&entity_id, "(%u) %s", entity->GetId(),
                            entity->GetName().c_str()))
        {
            for (auto& component_entry : entity->GetComponents())
            {
                const uint32_t& component_id =
                    component_entry.component->GetId();
                std::string name(component_entry.component->GetName());

                if (ImGui::TreeNode(&component_id, "%s", name.c_str(),
                                    entity->GetId()))
                {
                    ImGui::PushID(component_id);
                    ImGui::Indent(kComponentGuiIndent);
                    component_entry.component->OnDebugGui();
                    ImGui::Unindent(kComponentGuiIndent);
                    ImGui::PopID();
                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }
    }
}
