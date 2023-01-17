#pragma once

#include "engine/core/debug/Assert.h"
#include "engine/core/event/GlobalEventDispatcher.h"

class OnGuiEvent
{
  public:
    struct Data
    {
    };

    class ISubscriber
    {
      public:
        virtual void OnGui() = 0;
    };

    STATIC_ASSERT_INTERFACE(ISubscriber);
};

template <>
inline void GlobalEventDispatcher::Emit<OnGuiEvent>(const OnGuiEvent::Data& data)
{
}

template <>
inline void GlobalEventDispatcher::Subscribe<OnGuiEvent>(
    OnGuiEvent::ISubscriber* subscriber)
{
}
