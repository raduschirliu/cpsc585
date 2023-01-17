#pragma once

#include <cstdint>
#include <functional>
#include <vector>

using EventSubscriberId = uint32_t;

template <class Res, class... ArgTypes>
class EventSource
{
  public:
    using SubscriberCallback = std::function<Res(ArgTypes...)>;

    struct Subscriber
    {
        EventSubscriberId id;
        SubscriberCallback callback;
    };

    EventSource() : next_subscriber_id_(0), subscribers_{}
    {
    }

    void Emit(ArgTypes... args) const
    {
        for (auto& subscriber : subscribers_)
        {
            subscriber.callback(std::forward<decltype(args)>(args)...);
        }
    }

    EventSubscriberId Subscribe(SubscriberCallback handler)
    {
        subscribers_.push_back(
            Subscriber{.id = next_subscriber_id_, .callback = handler});
    }

    void Unsubscribe(EventSubscriberId id)
    {
        for (auto iter : subscribers_.begin())
        {
            if (iter->id == id)
            {
                subscribers_.erase(iter);
                return;
            }
        }
    }

  private:
    EventSubscriberId next_subscriber_id_;
    std::vector<Subscriber> subscribers_;
};
