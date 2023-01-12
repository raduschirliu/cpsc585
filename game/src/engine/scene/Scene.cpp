#include "engine/scene/Scene.h"

#include "engine/scene/Entity.h"

using std::make_unique;
using std::string_view;

Scene::Scene(string_view name, ServiceProvider& service_provider)
    : name_(name),
      entities_{},
      service_provider_(service_provider),
      component_builder_(service_provider)
{
}

Entity& Scene::AddEntity()
{
    auto entity = make_unique<Entity>();
    entity->SetScene(this);
    entities_.push_back(std::move(entity));

    return *entities_.back();
}

const ComponentBuilder* Scene::GetComponentBuilder() const
{
    return &component_builder_;
}
