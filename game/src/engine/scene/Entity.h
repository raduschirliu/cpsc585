#pragma once

#include <memory>
#include <string>
#include <vector>

#include "engine/core/debug/Assert.h"
#include "engine/scene/Component.h"

class Entity
{
  public:
    template <class ComponentType>
    ComponentType& AddComponent()
    {
        ASSERT_MSG(scene_, "Entity must belong to a scene to add a component");

        std::unique_ptr<ComponentType> component =
            scene_->GetComponentBuilder().Build<ComponentType>();
        components_.push_back(std::move(component));
    }

  private:
    size_t id_;
    std::string name_;
    Scene* scene_;
    std::vector<std::unique_ptr<IComponent>> components_;
};