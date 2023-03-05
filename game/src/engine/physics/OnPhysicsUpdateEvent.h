#pragma once

#include "engine/core/debug/Assert.h"
#include "engine/core/event/EventBus.h"
#include "engine/core/event/EventDispatcher.h"
#include "engine/core/math/Timestep.h"

struct OnPhysicsUpdateEvent : public IEvent
{
    Timestep step;
};

template <>
class IEventSubscriber<OnPhysicsUpdateEvent>
{
  public:
    virtual void OnPhysicsUpdate(const Timestep& timestep) = 0;
};

STATIC_ASSERT_INTERFACE(IEventSubscriber<OnPhysicsUpdateEvent>);

template <>
inline void EventDispatcher::Dispatch<OnPhysicsUpdateEvent>(
    IEventSubscriber<OnPhysicsUpdateEvent>* subscriber,
    const OnPhysicsUpdateEvent* event)
{
    ASSERT_MSG(event != nullptr,
               "OnPhysicsUpdate should have valid event data");
    subscriber->OnPhysicsUpdate(event->step);
}
