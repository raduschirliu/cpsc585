#pragma once

#include "engine/core/event/Event.h"
#include "engine/core/event/EventDispatcher.h"

class EventHandler
{
  public:
    void Exec()
    {
        DispatchCall(nullptr);
    }

    void Exec(const IEvent* event)
    {
        DispatchCall(event);
    }

  private:
    virtual void DispatchCall(const IEvent* event) = 0;
};

template <class EventType>
class ComponentEventSubscriber : public EventHandler
{
  public:
    ComponentEventSubscriber(IEventSubscriber<EventType>* instance)
        : instance_(instance)
    {
    }

  private:
    IEventSubscriber<EventType>* instance_;

    void DispatchCall(const IEvent* event) override
    {
        EventDispatcher::Dispatch<EventType>(
            instance_, static_cast<const EventType*>(event));
    }
};