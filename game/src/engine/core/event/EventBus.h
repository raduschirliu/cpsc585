#pragma once

#include <functional>
#include <memory>
#include <object_ptr.hpp>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "engine/core/event/Event.h"
#include "engine/core/event/EventHandler.h"

class EventBus;

class EventBus
{
  public:
    EventBus() : subscribers_{}, downstream_(nullptr)
    {
    }

    template <class EventType>
    void Publish()
    {
        Publish<EventType>(nullptr);
    }

    template <class EventType>
    void Publish(const EventType* event)
    {
        std::type_index key = typeid(EventType);

        for (auto& subscriber : subscribers_[key])
        {
            subscriber->Exec(event);
        }

        // Forward event to all downstream busses
        if (downstream_)
        {
            downstream_->Publish<EventType>(event);
        }
    }

    template <class EventType>
    void Subscribe(IEventSubscriber<EventType>* instance)
    {
        std::type_index key = typeid(EventType);

        auto subscriber =
            std::make_unique<ComponentEventSubscriber<EventType>>(instance);
        subscribers_[key].push_back(std::move(subscriber));
    }

    void SetDownstream(EventBus* event_bus)
    {
        downstream_ = event_bus;
    }

  private:
    std::unordered_map<std::type_index,
                       std::vector<std::unique_ptr<EventHandler>>>
        subscribers_;
    jss::object_ptr<EventBus> downstream_;
};
