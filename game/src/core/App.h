#pragma once

#include <memory>

#include "core/gfx/Window.h"

class App : public std::enable_shared_from_this<App>,
            public IWindowEventListener
{
  public:
    App();

    void Run();

  private:
    Window window_;
};