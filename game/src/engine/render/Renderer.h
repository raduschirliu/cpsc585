#pragma once

#include "engine/gfx/Window.h"

class Renderer
{
  public:
    Renderer(const Window& window);

    void Init();
    void RenderFrame();
    void Cleanup();

  private:
    const Window& window_;

    void RenderGui();
};