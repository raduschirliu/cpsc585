#include "engine/scene/Scene.h"

#include "engine/scene/Entity.h"

using std::make_unique;
using std::string_view;

Scene::Scene(string_view name, ServiceProvider& service_provider,
             EventBus& event_bus)
    : name_(name),
      entities_{},
      service_provider_(service_provider),
      event_bus_(event_bus)
{
}

Entity& Scene::AddEntity()
{
    auto entity = make_unique<Entity>();
    entity->SetScene(this);
    entities_.push_back(std::move(entity));

    return *entities_.back();
}

ComponentInitializer Scene::CreateComponentInitializer(Entity& entity) const
{
    return ComponentInitializer{.service_provider = service_provider_,
                                .event_bus = event_bus_,
                                .entity = entity};
}