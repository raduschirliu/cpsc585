#pragma once

#include <string_view>

#include "engine/core/debug/Assert.h"
#include "engine/core/event/EventBus.h"
#include "engine/service/ServiceProvider.h"

struct ComponentInitializer
{
    ServiceProvider& service_provider;
    EventBus& event_bus;
};

class Component
{
  public:
    Component();

    virtual void Init(ComponentInitializer& initializer) = 0;
    // virtual void Start() = 0;
    virtual std::string_view GetName() const = 0;
  
  protected:
    EventBus& GetEventBus();

  private:
    EventBus* event_bus_;
};
