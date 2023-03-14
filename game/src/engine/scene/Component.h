#pragma once

#include <object_ptr.hpp>
#include <string_view>

#include "engine/core/debug/Assert.h"
#include "engine/core/event/EventBus.h"
#include "engine/scene/EventData.h"
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
    Entity& GetEntity();

    virtual void OnInit(const ServiceProvider& service_provider) = 0;
    virtual void OnStart();
    virtual void OnDestroy();
    virtual void OnTriggerEnter(const OnTriggerEvent& data);
    virtual void OnTriggerExit(const OnTriggerEvent& data);
    virtual void OnDebugGui();
    virtual std::string_view GetName() const = 0;

    const uint32_t& GetId() const;

  protected:
    void ManageEventSub(uint32_t subscription_id);
    EventBus& GetEventBus();

  private:
    uint32_t id_;
    jss::object_ptr<Entity> entity_;
    jss::object_ptr<EventBus> event_bus_;
    std::vector<uint32_t> event_sub_ids_;
};
