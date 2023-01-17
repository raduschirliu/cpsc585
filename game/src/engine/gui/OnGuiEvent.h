#pragma once

#include "engine/core/debug/Assert.h"
#include "engine/core/event/GlobalEventDispatcher.h"

struct OnGuiEvent : public IEvent
{
    class ISubscriber
    {
      public:
        virtual void OnGui() = 0;
    };

    STATIC_ASSERT_INTERFACE(ISubscriber);

    static void Dispatch(ISubscriber* subscriber, OnGuiEvent* event)
    {
        ASSERT_MSG(event == nullptr, "This event should not take a parameter");
        subscriber->OnGui();
    }
};

template <>
