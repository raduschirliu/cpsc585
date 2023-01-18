#pragma once

#include <string_view>

#include "engine/core/debug/Assert.h"
#include "engine/core/event/EventBus.h"
#include "engine/service/ServiceProvider.h"

class Entity;

struct ComponentInitializer
{
    ServiceProvider& service_provider;
    EventBus& event_bus;
    Entity& entity;
};

class Component
{
  public:
    Component();

    void DispatchInit(ComponentInitializer& initializer);
    Entity& GetEntity() const;

    virtual void Init(const ServiceProvider& service_provider) = 0;
    virtual void Start();
    virtual void Cleanup();
    virtual std::string_view GetName() const = 0;
  
  protected:
    EventBus& GetEventBus();

  private:
    Entity* entity_;
    EventBus* event_bus_;
};
