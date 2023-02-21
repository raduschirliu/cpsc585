#include "engine/scene/Scene.h"

#include "engine/scene/Entity.h"

using std::make_unique;
using std::string;
using std::unique_ptr;
using std::vector;

Scene::Scene(const string& name, ServiceProvider& service_provider)
    : name_(name),
      entities_{},
      service_provider_(service_provider),
      event_bus_()
{
}

Entity& Scene::AddEntity(const string& name)
{
    auto entity = make_unique<Entity>(name);
    entity->SetScene(this);
    entities_.push_back(std::move(entity));

    return *entities_.back();
}

ComponentInitializer Scene::CreateComponentInitializer(Entity& entity)
{
    return ComponentInitializer{.service_provider = service_provider_,
                                .event_bus = event_bus_,
                                .entity = entity};
}

vector<unique_ptr<Entity>>& Scene::GetEntities()
{
    return entities_;
}

EventBus& Scene::GetEventBus()
{
    return event_bus_;
}

const string& Scene::GetName() const
{
    return name_;
}
