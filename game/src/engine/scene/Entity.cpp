#include "engine/scene/Entity.h"

#include "engine/scene/Scene.h"

using std::unique_ptr;

static uint32_t kNextEntityId = 0;

Entity::Entity()
    : id_(kNextEntityId),
      name_("Entity"),
      scene_(nullptr),
      components_{}
{
    kNextEntityId += 1;
}

void Entity::SetScene(Scene* scene)
{
    scene_ = scene;
}

void Entity::InitComponent(Component& component)
{
    ComponentInitializer initializer =
        scene_->CreateComponentInitializer(*this);

    component.Init(initializer);
}