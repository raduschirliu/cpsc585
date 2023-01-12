#pragma once

#include <concepts>
#include <memory>
#include <string>
#include <vector>

#include "engine/core/debug/Assert.h"
#include "engine/scene/Component.h"
#include "engine/scene/ComponentBuilder.h"

class Scene;

class Entity
{
  public:
    template <class ComponentType>
        requires std::derived_from<ComponentType, IComponent>
    void AddComponent()
    {
        ASSERT_MSG(scene_, "Entity must belong to a scene to add a component");
        ASSERT_MSG(component_builder_,
                   "Entity must have a valid component builder reference");

        std::unique_ptr<ComponentType> component =
            component_builder_->Build<ComponentType>();
        component->Init();

        components_.push_back(std::move(component));
    }

    void SetScene(Scene* scene);

  private:
    size_t id_;
    std::string name_;
    Scene* scene_;
    const ComponentBuilder* component_builder_;
    std::vector<std::unique_ptr<IComponent>> components_;
};