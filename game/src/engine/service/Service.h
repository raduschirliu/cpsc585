#pragma once

#include <object_ptr.hpp>
#include <string_view>

#include "engine/core/debug/Assert.h"
#include "engine/core/event/EventBus.h"

class ServiceProvider;
class Window;

struct ServiceInitializer
{
    Window& window;
    EventBus& event_bus;
    ServiceProvider& service_provider;
};

class Service
{
  public:
    void Init(ServiceInitializer& initializer);

    virtual void OnInit();
    virtual void OnStart(ServiceProvider& service_provider);
    virtual void OnUpdate() = 0;
    virtual void OnCleanup();

    virtual std::string_view GetName() const = 0;

  protected:
    Window& GetWindow();
    EventBus& GetEventBus();

  private:
    jss::object_ptr<Window> window_;
    jss::object_ptr<EventBus> event_bus_;
};
