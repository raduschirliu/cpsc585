#include "core/scene/SceneManager.h"

#include <fmt/format.h>
#include <imgui.h>

#include <cassert>

#include "core/Input.h"

using glm::ivec2;
using std::make_pair;
using std::make_shared;
using std::nullopt;
using std::shared_ptr;

SceneManager::SceneManager()
    : scenes_(),
      active_scene_(nullopt),
      show_demo_window_(false),
      window_size_(ivec2(0, 0)),
      last_update_time_(0)
{
}

void SceneManager::UpdateScene()
{
    double current_time = glfwGetTime();
    double delta = current_time - last_update_time_;
    last_update_time_ = current_time;

    Input::Update();

    if (active_scene_)
    {
        ActiveScene().scene->UpdateScene(static_cast<float>(delta));
    }
}

void SceneManager::RenderScene()
{
    if (active_scene_)
    {
        ActiveScene().scene->RenderScene();
    }
}

void SceneManager::RenderGui()
{
    if (show_demo_window_)
    {
        ImGui::ShowDemoWindow(&show_demo_window_);
    }

    ImGui::Begin("Main Gui");
    ImGui::BeginTabBar("Main Gui Tabs");

    DrawSceneTab();

    if (active_scene_)
    {
        SceneData& data = ActiveScene();

        if (ImGui::BeginTabItem(data.name.c_str()))
        {
            data.scene->RenderGui();
            ImGui::EndTabItem();
        }
    }

    ImGui::EndTabBar();
    ImGui::End();
}

void SceneManager::RegisterScene(std::string name, std::unique_ptr<Scene> scene)
{
    scenes_.push_back(SceneData(name, move(scene)));
}

void SceneManager::SetActiveScene(std::string name)
{
    for (size_t i = 0; i < scenes_.size(); i++)
    {
        SceneData& scene = scenes_[i];

        if (scene.name == name)
        {
            SetActiveScene(i);
            return;
        }
    }
}

void SceneManager::SetActiveScene(size_t index)
{
    ASSERT_MSG(index < scenes_.size(), "Index larger than amount of scenes");
    active_scene_ = index;
    ActiveScene().scene->Init();
}

void SceneManager::OnKeyEvent(int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS && key == GLFW_KEY_F1)
    {
        show_demo_window_ = !show_demo_window_;
        return;
    }

    // Check if ImGui is using this input first, then forward to active scene
    ImGuiIO& io = ImGui::GetIO();

    if (!io.WantCaptureKeyboard)
    {
        Input::OnKeyEvent(key, scancode, action, mods);

        if (active_scene_)
        {
            if (ActiveScene().scene->OnKeyEvent(key, scancode, action, mods))
            {
                return;
            }
        }
    }
}

void SceneManager::OnCursorMove(double xpos, double ypos)
{
    // These will never be non-int anyway... even a float is kinda overkill, oh
    // well Make window coordinates match OpenGL coordinates (+X = right, Y+ =
    // up)
    float x = static_cast<float>(xpos);
    float y = static_cast<float>(window_size_.y) - static_cast<float>(ypos);

    Input::OnCursorMove(x, y);

    if (active_scene_)
    {
        ActiveScene().scene->OnCursorMove(x, y);
    }
}

void SceneManager::OnMouseButtonEvent(int button, int action, int mods)
{
    // Check if ImGui is using this input first, then forward to active scene
    ImGuiIO& io = ImGui::GetIO();

    if (!io.WantCaptureMouse)
    {
        Input::OnMouseButtonEvent(button, action, mods);

        if (active_scene_)
        {
            ActiveScene().scene->OnMouseButtonEvent(button, action, mods);
        }
    }
}

void SceneManager::OnWindowSizeChanged(int width, int height)
{
    ivec2 size(width, height);

    for (auto& scene : scenes_)
    {
        scene.scene->SetWindowSize(size);
    }

    window_size_ = size;
}

SceneManager::SceneData& SceneManager::ActiveScene()
{
    ASSERT_MSG(active_scene_, "No scene is currently active");
    return scenes_[active_scene_.value()];
}

void SceneManager::DrawSceneTab()
{
    if (ImGui::BeginTabItem("Scenes"))
    {
        ImGui::Text("Scene selection");

        for (size_t i = 0; i < scenes_.size(); i++)
        {
            SceneData& scene = scenes_[i];
            bool is_active = active_scene_ && active_scene_.value() == i;

            if (is_active)
            {
                ImGui::BeginDisabled();
            }

            ImGui::PushID(static_cast<int>(i));
            if (ImGui::Button(" > "))
            {
                SetActiveScene(i);
            }
            ImGui::PopID();

            if (is_active)
            {
                ImGui::EndDisabled();
            }

            ImGui::SameLine();
            ImGui::SameLine();
            ImGui::Text("%s", scene.name.c_str());
        }
        ImGui::EndTabItem();
    }
}
