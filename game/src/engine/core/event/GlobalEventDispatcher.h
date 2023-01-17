#pragma once

#include <functional>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "engine/core/event/EventSource.h"

class GlobalEventDispatcher
{
  public:
    template <class Event>
    void Emit(Event::Args args) = delete;

    template <class Event>
    void Subscribe(Event::ISubscriber* subscriber) = delete;

  private:
    std::unordered_map<std::string_view, std::function<void(void)>>
        event_sources_;
};
