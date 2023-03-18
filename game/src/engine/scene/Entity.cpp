#include "engine/scene/Entity.h"

#include "engine/scene/Scene.h"

using jss::object_ptr;
using std::string;
using std::unique_ptr;
using std::vector;

static uint32_t kNextEntityId = 0;

Entity::Entity(const std::string& name)
    : id_(kNextEntityId),
      name_(name),
      scene_(nullptr),
      components_{}
{
    kNextEntityId += 1;
}

vector<Entity::ComponentEntry>& Entity::GetComponents()
{
    return components_;
}

void Entity::SetName(const string& name)
{
    name_ = name;
}

void Entity::Destroy()
{
    ASSERT_MSG(scene_, "Entity must have valid scene");
    scene_->DestroyEntity(id_);
}

void Entity::OnDestroy()
{
    for (auto& entry : components_)
    {
        entry.component->OnDestroy();
    }
}

void Entity::SetScene(object_ptr<Scene> scene)
{
    scene_ = scene;
}

void Entity::InitComponent(Component& component)
{
    ComponentInitializer initializer =
        scene_->CreateComponentInitializer(*this);

    component.Init(initializer);
}

const uint32_t& Entity::GetId() const
{
    return id_;
}

const string& Entity::GetName() const
{
    return name_;
}
