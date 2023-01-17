#pragma once

class IEvent
{
};

class EventSubscriber
{
  public:
    void Exec()
    {
        DispatchCall(nullptr);
    }

    void Exec(IEvent* event)
    {
        DispatchCall(event);
    }

  private:
    virtual void DispatchCall(IEvent* event) = 0;
};

template <class EventType>
class ComponentEventSubscriber : public EventSubscriber
{
  public:
    ComponentEventSubscriber(EventType::ISubscriber* instance)
        : instance_(instance)
    {
    }

  private:
    EventType::ISubscriber* instance_;

    void DispatchCall(IEvent* event) override
    {
        EventType::Dispatch(instance_, static_cast<EventType*>(event));
    }
};