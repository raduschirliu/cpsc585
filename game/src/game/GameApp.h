#pragma once

#include "engine/App.h"

class GameApp : public App
{
  public:
    GameApp();

  protected:
    // From App
    void OnInit() override;
    void OnStart() override;
};
