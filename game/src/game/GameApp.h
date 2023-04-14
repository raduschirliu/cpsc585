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
    void LoadMainMenuScene(Scene& scene);
    void LoadHowToPlayScene(Scene& scene);
    void LoadPowerupsScene(Scene& scene);
    void LoadSettingScene(Scene& scene);
};
