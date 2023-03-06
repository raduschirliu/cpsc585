#pragma once

#include "engine/App.h"
#include "engine/core/Colors.h"

class GameApp : public App
{
  public:
    GameApp();

  protected:
    // From App
    void OnInit() override;
    void OnStart() override;
    void OnSceneLoaded(Scene& scene) override;

  private:
    void LoadTestScene(Scene& scene);
    void LoadTrack1Scene(Scene& scene);
    Entity& CreatePlayer(Scene& scene, const std::string& name, bool human,
                         const glm::vec3& position,
                         const glm::vec3& orientation_euler_degrees,
                         const Color3& color);
};
