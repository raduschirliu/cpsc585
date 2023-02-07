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
    Scene(const std::string& name, ServiceProvider& service_provider);

    Entity& AddEntity(const std::string& name = "Entity");
    EventBus& GetEventBus();
    ComponentInitializer CreateComponentInitializer(Entity& entity);
    std::vector<std::unique_ptr<Entity>>& GetEntities();

    const std::string& GetName() const;

  private:
    std::string name_;
    std::vector<std::unique_ptr<Entity>> entities_;
    ServiceProvider& service_provider_;
    EventBus event_bus_;
};
