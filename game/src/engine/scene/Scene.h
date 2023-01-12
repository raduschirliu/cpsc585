#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "engine/scene/ComponentBuilder.h"
#include "engine/scene/Entity.h"
#include "engine/service/ServiceProvider.h"

class Scene
{
  public:
    Scene(std::string_view name, ServiceProvider& service_provider);

    Entity& AddEntity();

    const ComponentBuilder* GetComponentBuilder() const;

  private:
    std::string name_;
    // TODO: Since entities are stored in a vector, there is the potential of
    // having pointers to them that are incorrect if the vector resizes...
    std::vector<std::unique_ptr<Entity>> entities_;
    ServiceProvider& service_provider_;
    ComponentBuilder component_builder_;
};
