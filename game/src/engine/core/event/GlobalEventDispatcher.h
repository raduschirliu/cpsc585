#pragma once

#include <functional>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "engine/core/event/EventSubscriber.h"

class GlobalEventDispatcher
{
  public:
    template <class EventType>
    void Publish()
    {
        Publish<EventType>(nullptr);
    }

    template <class EventType>
    void Publish(EventType* event)
    {
        std::type_index key = typeid(EventType);

        for (auto& subscriber : subscribers_[key])
        {
            subscriber->Exec(event);
        }
    }

    template <class EventType>
    void Subscribe(EventType::ISubscriber* instance)
    {
        std::type_index key = typeid(EventType);

        auto subscriber =
            std::make_unique<ComponentEventSubscriber<EventType>>(instance);
        subscribers_[key].push_back(std::move(subscriber));
    }

  private:
    std::unordered_map<std::type_index,
                       std::vector<std::unique_ptr<EventSubscriber>>>
        subscribers_;
};
