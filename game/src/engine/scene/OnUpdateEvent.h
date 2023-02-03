#pragma once

#include "engine/core/debug/Assert.h"
#include "engine/core/event/EventBus.h"
#include "engine/core/event/EventDispatcher.h"
#include "engine/core/math/Timestep.h"

struct OnUpdateEvent : public IEvent
{
    Timestep delta_time;
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
    IEventSubscriber<OnUpdateEvent>* subscriber, const OnUpdateEvent* event)
{
    ASSERT_MSG(event != nullptr, "OnUpdate should have valid event data");
    subscriber->OnUpdate();
}
