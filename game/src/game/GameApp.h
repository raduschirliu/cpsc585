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
    void GetCheckpoints();
    void OnSceneLoaded(Scene& scene) override;

  private:
    void LoadTestScene(Scene& scene);
    void LoadTrack1Scene(Scene& scene);
};
