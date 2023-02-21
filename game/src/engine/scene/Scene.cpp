#include "engine/scene/Scene.h"

#include "engine/scene/Entity.h"

using std::make_unique;
using std::string;
using std::unique_ptr;
using std::vector;

Scene::Scene(const string& name, ServiceProvider& service_provider)
    : name_(name),
      active_(false),
      entities_{},
      service_provider_(service_provider),
      event_bus_()
{
}

void Scene::DestroyEntity(uint32_t id)
{
    auto iter = entities_.begin();

    while (iter != entities_.end())
    {
        Entity* entity = iter->get();

        if (entity->GetId() == id)
        {
            entity->OnDestroy();
            entities_.erase(iter);
            return;
        }

        iter++;
    }

    ASSERT_ALWAYS("Entity does not exist");
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

void Scene::Load()
{
    ASSERT_MSG(!active_, "Cannot load a scene that's already active");
    active_ = true;
}

void Scene::Unload()
{
    ASSERT_MSG(active_, "Cannot unload a scene that's not active");
    active_ = false;

    for (auto& entity : entities_)
    {
        // Call the OnDestroy callback instead of Destroy, as Destroy will call
        // Scene again to remove the entity and cause nullptr issues
        entity->OnDestroy();
    }

    entities_.clear();
    event_bus_.ClearSubscribers();
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
