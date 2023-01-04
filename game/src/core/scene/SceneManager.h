#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "Scene.h"
#include "core/ShaderManager.h"
#include "core/gfx/Window.h"

class SceneManager : public IWindowEventListener
{
  public:
    SceneManager();

    void UpdateScene();
    void RenderScene();
    void RenderGui();
    void RegisterScene(std::string name, std::unique_ptr<Scene> scene);
    void SetActiveScene(std::string name);
    void SetActiveScene(size_t index);

    // From IWindowEventListener
    void OnKeyEvent(int key, int scancode, int action, int mods) override;
    void OnCursorMove(double xpos, double ypos) override;
    void OnMouseButtonEvent(int button, int action, int mods) override;
    void OnWindowSizeChanged(int width, int height) override;

  private:
    struct SceneData
    {
        std::string name;
        std::unique_ptr<Scene> scene;

        SceneData(std::string name, std::unique_ptr<Scene> scene)
            : name(name),
              scene(std::move(scene)){};
    };

    std::vector<SceneData> scenes_;
    std::optional<size_t> active_scene_;
    bool show_demo_window_;
    glm::ivec2 window_size_;
    double last_update_time_;

    SceneData& ActiveScene();
    void DrawSceneTab();
};