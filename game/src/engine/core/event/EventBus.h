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
    // Must be declared at top since it's used in the public section below
  private:
    static inline uint32_t kNextId = 0;

  public:
    struct Subscription
    {
        uint32_t id;
        std::unique_ptr<EventHandler> handler;
    };

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
            subscriber.handler->Exec(event);
        }

        // Forward event to all downstream busses
        if (downstream_)
        {
            downstream_->Publish<EventType>(event);
        }
    }

    template <class EventType>
    uint32_t Subscribe(IEventSubscriber<EventType>* instance)
    {
        std::type_index key = typeid(EventType);

        Subscription sub = {
            .id = kNextId,
            .handler = std::make_unique<ComponentEventSubscriber<EventType>>(
                instance)};

        kNextId++;
        subscribers_[key].push_back(std::move(sub));

        return sub.id;
    }

    void Unsubscribe(uint32_t id)
    {
        // This is super inefficient, but oeh well...
        auto sub_types_iter = subscribers_.begin();

        while (sub_types_iter != subscribers_.end())
        {
            auto sub_iter = sub_types_iter->second.begin();

            while (sub_iter != sub_types_iter->second.end())
            {
                if (sub_iter->id == id)
                {
                    sub_types_iter->second.erase(sub_iter);
                    return;
                }

                sub_iter++;
            }

            sub_types_iter++;
        }
    }

    void ClearSubscribers()
    {
        subscribers_.clear();
    }

    void SetDownstream(EventBus* event_bus)
    {
        downstream_ = event_bus;
    }

  private:
    std::unordered_map<std::type_index, std::vector<Subscription>> subscribers_;
    jss::object_ptr<EventBus> downstream_;
};
