#pragma once

#include "engine/core/debug/Assert.h"
#include "engine/core/event/EventHandler.h"

class EventDispatcher
{
  public:
    template <class EventType>
    static void Dispatch(IEventSubscriber<EventType>* subscriber,
                         const EventType* event)
    {
        ASSERT_MSG(false, "Unspecialized Dispatch template should not be used");
    }
};