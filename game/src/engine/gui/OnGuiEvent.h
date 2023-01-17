#pragma once

#include "engine/core/debug/Assert.h"
#include "engine/core/event/EventBus.h"
#include "engine/core/event/EventDispatcher.h"

struct OnGuiEvent : public IEvent
{
};

template <>
class IEventSubscriber<OnGuiEvent>
{
  public:
    virtual void OnGui() = 0;
};

STATIC_ASSERT_INTERFACE(IEventSubscriber<OnGuiEvent>);

template <>
inline void EventDispatcher::Dispatch<OnGuiEvent>(
    IEventSubscriber<OnGuiEvent>* subscriber, OnGuiEvent* event)
{
    ASSERT_MSG(event == nullptr, "OnGui should not receive any event data");
    subscriber->OnGui();
}
