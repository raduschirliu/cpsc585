#include "engine/scene/Entity.h"

#include "engine/scene/Scene.h"

void Entity::SetScene(Scene* scene)
{
    scene_ = scene;
    component_builder_ = scene_->GetComponentBuilder();
}