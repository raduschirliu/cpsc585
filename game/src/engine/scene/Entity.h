#pragma once

#include <concepts>
#include <memory>
#include <object_ptr.hpp>
#include <string>
#include <typeindex>
#include <vector>

#include "engine/core/debug/Assert.h"
#include "engine/scene/Component.h"

class Scene;

class Entity
{
  public:
    struct ComponentEntry
    {
        std::type_index type;
        std::unique_ptr<Component> component;
    };

    Entity();

    template <class ComponentType>
        requires std::derived_from<ComponentType, Component>
    ComponentType& AddComponent()
    {
        ASSERT_MSG(scene_, "Entity must belong to a Scene to add a Component");
        ASSERT_MSG(
            !HasComponent<ComponentType>(),
            "Cannot have two Components of the same type on the same Entity");

        ComponentEntry entry{.type = std::type_index(typeid(ComponentType)),
                             .component = std::make_unique<ComponentType>()};

        ComponentType& component_ref =
            static_cast<ComponentType&>(*entry.component);
        InitComponent(component_ref);

        components_.push_back(std::move(entry));

        // Returning the ref here is okay despite the move above, since it's a
        // ref to a ComponentType allocated on the heap. Moving the pointer does
        // not change it's address, therefore this is safe
        return component_ref;
    }

    template <class ComponentType>
        requires std::derived_from<ComponentType, Component>
    ComponentType& GetComponent() const
    {
        std::type_index key = typeid(ComponentType);

        for (auto& entry : components_)
        {
            if (entry.type == key)
            {
                return static_cast<ComponentType&>(*entry.component);
            }
        }

        // This should never happen at runtime, so throw error and crash
        throw new std::exception("Component does not exist on Entity");
    }

    // clang-format off
    // clang-format keeps trying to put the return type on a different line...

    template <class ComponentType>
        requires std::derived_from<ComponentType, Component>
    bool HasComponent()
    {
        // clang-format on
        std::type_index key = typeid(ComponentType);

        for (auto& entry : components_)
        {
            if (entry.type == key)
            {
                return true;
            }
        }

        return false;
    }

    void SetScene(jss::object_ptr<Scene> scene);
    uint32_t GetId() const;

  protected:
    void InitComponent(Component& component);

  private:
    uint32_t id_;
    std::string name_;
    jss::object_ptr<Scene> scene_;
    std::vector<ComponentEntry> components_;
};