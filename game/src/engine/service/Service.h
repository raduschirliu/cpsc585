#pragma once

#include <object_ptr.hpp>
#include <string_view>

#include "engine/core/debug/Assert.h"
#include "engine/core/event/EventBus.h"

class ServiceProvider;
class Window;
class Scene;
class App;

struct ServiceInitializer
{
    Window& window;
    ServiceProvider& service_provider;
    App& app;
};

class Service
{
  public:
    void Init(ServiceInitializer& initializer);

    virtual void OnInit();
    virtual void OnSceneLoaded(Scene& scene);
    virtual void OnSceneUnloaded(Scene& scene);
    virtual void OnStart(ServiceProvider& service_provider);
    virtual void OnUpdate() = 0;
    virtual void OnCleanup();

    virtual std::string_view GetName() const = 0;

  protected:
    Window& GetWindow();
    EventBus& GetEventBus();
    App& GetApp();

  private:
    jss::object_ptr<Window> window_;
    jss::object_ptr<EventBus> event_bus_;
    jss::object_ptr<App> app_;
};
