#include "engine/scene/Scene.h"

using std::string_view;

Scene::Scene(string_view name, ServiceProvider& service_provider)
    : name_(name),
      entities_{},
      service_provider_(service_provider),
      component_builder_(service_provider)
{
}

const ComponentBuilder& Scene::GetComponentBuilder() const
{
    return component_builder_;
}
