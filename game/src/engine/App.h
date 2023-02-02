#pragma once

#include <memory>
#include <string_view>

#include "engine/core/gfx/Window.h"
#include "engine/scene/SceneList.h"
#include "engine/scene/Scene.h"
#include "engine/service/ServiceProvider.h"

class App : public std::enable_shared_from_this<App>,
            public IWindowEventListener
{
  public:
    App();

    void Run();

    // From IWindowEventListener
    void OnKeyEvent(int key, int scancode, int action, int mods) override;
    void OnMouseButtonEvent(int button, int action, int mods) override;
    void OnCursorMove(double xpos, double ypos) override;
    void OnScroll(double xoffset, double yoffset) override;
    void OnWindowSizeChanged(int width, int height) override;

  protected:
    virtual void OnInit();
    virtual void OnStart();
    virtual void OnCleanup();

    template <class ServiceType>
        requires std::derived_from<ServiceType, Service>
    void AddService()
    {
        service_provider_.AddService(std::make_unique<ServiceType>());
    }

    Scene& AddScene(std::string_view name);
    void SetActiveScene(std::string_view name);
    Window& GetWindow();
    EventBus& GetActiveEventBus();

  private:
    bool running_;
    Window window_;
    ServiceProvider service_provider_;
    SceneList scene_list_;

    void PerformGameLoop();
};