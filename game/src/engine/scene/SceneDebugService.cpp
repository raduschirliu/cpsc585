#include "engine/scene/SceneDebugService.h"

#include <imgui.h>

#include <string>

#include "engine/App.h"
#include "engine/core/debug/Log.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/service/ServiceProvider.h"

using std::string_view;

void SceneDebugService::OnInit()
{
}

void SceneDebugService::OnStart(ServiceProvider& service_provider)
{
    GetEventBus().Subscribe<OnGuiEvent>(this);
}

void SceneDebugService::OnUpdate()
{
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
    ImGui::Begin("Scene");

    auto& scene = GetApp().GetSceneList().GetActiveScene();

    for (auto& entity : scene.GetEntities())
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
                    component_entry.component->OnDebugGui();
                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }
    }

    ImGui::End();
}