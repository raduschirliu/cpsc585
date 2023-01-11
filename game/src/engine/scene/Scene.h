#pragma once

#include <string>
#include <vector>

#include "engine/scene/ComponentBuilder.h"
#include "engine/scene/Entity.h"
#include "engine/service/ServiceProvider.h"

class Scene
{
  public:
    Scene(std::string_view name, ServiceProvider& service_provider);

    const ComponentBuilder& GetComponentBuilder() const;

  private:
    std::string name_;
    std::vector<Entity> entities_;
    ServiceProvider& service_provider_;
    ComponentBuilder component_builder_;
};
