#pragma once

#include <memory>
#include <string_view>
#include <vector>

#include "engine/scene/Scene.h"

class SceneList
{
  public:
    SceneList();

    Scene& GetActiveScene();
    Scene& AddScene(std::unique_ptr<Scene> scene);
    void SetActiveScene(std::string_view name);

    bool HasScene(std::string_view name);
    bool HasActiveScene() const;
    const std::vector<std::unique_ptr<Scene>>& GetAllScenes() const;

  private:
    std::vector<std::unique_ptr<Scene>> scenes_;
    jss::object_ptr<Scene> active_scene_;
};
