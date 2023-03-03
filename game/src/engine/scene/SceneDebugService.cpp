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
    framerate_ = 0;
    prev_time_ = 0.0;
    frame_count_ = 0;
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

    double cur_time = glfwGetTime();
    frame_count_ += 1;

    if (cur_time - prev_time_ >= 1.0)
    {
        framerate_ = frame_count_;
        frame_count_ = 0;
        prev_time_ = cur_time;
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

    if (!ImGui::Begin("Inspector", &show_menu_))
    {
        ImGui::End();
        return;
    }

    ImGui::BeginTabBar("##Inspector Tabs");

    if (ImGui::BeginTabItem("General"))
    {
        DrawGeneralTab();
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Scenes"))
    {
        DrawSceneTab();
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Entities"))
    {
        DrawEntityTab();
        ImGui::EndTabItem();
    }

    ImGui::EndTabBar();
    ImGui::End();
}

void SceneDebugService::DrawGeneralTab()
{
    ImGui::Text("FPS: %d", framerate_);
    ImGui::Text("Active scene: %s", active_scene_->GetName().c_str());

    if (ImGui::Button("Reload Scene"))
    {
        Log::info("Reloading scene...");
        GetApp().SetActiveScene(active_scene_->GetName());
    }

    ImGui::Checkbox("Show ImGui demo", &show_demo_menu_);
    if (show_demo_menu_)
    {
        ImGui::ShowDemoWindow();
    }
}

void SceneDebugService::DrawEntityTab()
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

void SceneDebugService::DrawSceneTab()
{
    auto& scenes = GetApp().GetSceneList().GetAllScenes();

    for (auto& scene : scenes)
    {
        ImGui::PushID(scene->GetName().c_str());

        if (ImGui::Button("Load"))
        {
            Log::info("Changing scene: {}", scene->GetName());
            GetApp().SetActiveScene(scene->GetName());
        }

        ImGui::SameLine();
        ImGui::Text("%s", scene->GetName().c_str());

        ImGui::PopID();
    }
}
