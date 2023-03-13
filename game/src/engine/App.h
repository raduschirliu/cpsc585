#pragma once

#include <memory>
#include <string_view>

#include "engine/core/gfx/Window.h"
#include "engine/core/math/Timestep.h"
#include "engine/scene/Scene.h"
#include "engine/scene/SceneList.h"
#include "engine/service/ServiceProvider.h"

class App : public std::enable_shared_from_this<App>,
            public IWindowEventListener
{
  public:
    App();

    void Run();
    void SetActiveScene(const std::string& name);

    // From IWindowEventListener
    void OnKeyEvent(int key, int scancode, int action, int mods) override;
    void OnMouseButtonEvent(int button, int action, int mods) override;
    void OnCursorMove(double xpos, double ypos) override;
    void OnScroll(double xoffset, double yoffset) override;
    void OnWindowSizeChanged(int width, int height) override;
    void OnJoystickChangedEvent(int joystick_id, int event) override;

    const Timestep& GetDeltaTime() const;
    Window& GetWindow();
    EventBus& GetEventBus();
    SceneList& GetSceneList();

  protected:
    virtual void OnInit();
    virtual void OnStart();
    virtual void OnCleanup();
    virtual void OnSceneLoaded(Scene& scene);
    virtual void OnSceneUnloaded(Scene& scene);

    template <class ServiceType>
        requires std::derived_from<ServiceType, Service>
    void AddService()
    {
        service_provider_.AddService(std::make_unique<ServiceType>());
    }

    Scene& AddScene(const std::string& name);
    ServiceProvider& GetServiceProvider();

  private:
    bool running_;
    Window window_;
    ServiceProvider service_provider_;
    SceneList scene_list_;
    EventBus event_bus_;
    Timestep last_frame_, delta_time_;

    void PerformGameLoop();
    void CalculateDeltaTime();
};