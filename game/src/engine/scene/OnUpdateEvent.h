#pragma once

#include "engine/core/debug/Assert.h"
#include "engine/core/event/EventBus.h"
#include "engine/core/event/EventDispatcher.h"

struct OnUpdateEvent : public IEvent
{
};

template <>
class IEventSubscriber<OnUpdateEvent>
{
  public:
    virtual void OnUpdate() = 0;
};

STATIC_ASSERT_INTERFACE(IEventSubscriber<OnUpdateEvent>);

template <>
inline void EventDispatcher::Dispatch<OnUpdateEvent>(
    IEventSubscriber<OnUpdateEvent>* subscriber, OnUpdateEvent* event)
{
    ASSERT_MSG(event == nullptr, "OnUpdate should not receive any event data");
    subscriber->OnUpdate();
}
