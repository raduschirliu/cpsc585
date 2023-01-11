#pragma once

#include <memory>

#include "engine/service/ServiceProvider.h"

class ComponentBuilder
{
  public:
    ComponentBuilder(ServiceProvider& service_provider);

    template <class Component>
    std::unique_ptr<Component> Build()
    {
        // Default implementation for components that don't require
        // any services to be injected

        return std::make_unique<Component>();
    }

  private:
    ServiceProvider& service_provider_;
};
