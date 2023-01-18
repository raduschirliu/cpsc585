#pragma once

#include <object_ptr.hpp>
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

    void Init(ComponentInitializer& initializer);
    Entity& GetEntity() const;

    virtual void OnInit(const ServiceProvider& service_provider) = 0;
    virtual void OnStart();
    virtual void OnUpdate() = 0;
    virtual void OnCleanup();
    virtual std::string_view GetName() const = 0;

  protected:
    EventBus& GetEventBus();

  private:
    jss::object_ptr<Entity> entity_;
    jss::object_ptr<EventBus> event_bus_;
};
