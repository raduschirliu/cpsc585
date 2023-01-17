#pragma once

#include <memory>

#include "engine/core/event/GlobalEventDispatcher.h"
#include "engine/service/ServiceProvider.h"

class ComponentBuilder
{
  public:
    ComponentBuilder(ServiceProvider& service_provider,
                     GlobalEventDispatcher& event_dispatcher);

    template <class Component>
    std::unique_ptr<Component> Build() const
    {
        // Default implementation for components that don't require
        // any services to be injected, and do not listen to any events

        return std::make_unique<Component>();
    }

  private:
    ServiceProvider& service_provider_;
    GlobalEventDispatcher& event_dispatcher_;
};
