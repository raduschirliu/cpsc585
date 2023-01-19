#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "engine/scene/Entity.h"
#include "engine/service/ServiceProvider.h"

class Scene
{
  public:
    Scene(std::string_view name, ServiceProvider& service_provider,
          EventBus& event_bus);

    Entity& AddEntity();

    ComponentInitializer CreateComponentInitializer(Entity& entity) const;

  private:
    std::string name_;
    std::vector<std::unique_ptr<Entity>> entities_;
    ServiceProvider& service_provider_;
    EventBus& event_bus_;
};
