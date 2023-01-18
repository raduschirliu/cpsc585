#pragma once

#include <concepts>
#include <memory>
#include <string>
#include <vector>

#include "engine/core/debug/Assert.h"
#include "engine/scene/Component.h"

class Scene;

class Entity
{
  public:
    Entity();

    template <class ComponentType>
        requires std::derived_from<ComponentType, Component>
    void AddComponent()
    {
        ASSERT_MSG(scene_, "Entity must belong to a scene to add a component");

        auto component = std::make_unique<ComponentType>();
        InitComponent(*component);

        components_.push_back(std::move(component));
    }

    void SetScene(Scene* scene);

  protected:
    void InitComponent(Component& component);

  private:
    uint32_t id_;
    std::string name_;
    Scene* scene_;
    std::vector<std::unique_ptr<Component>> components_;
};