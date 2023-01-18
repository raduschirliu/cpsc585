#include "engine/scene/Component.h"

Component::Component() : event_bus_(nullptr)
{
}

EventBus& Component::GetEventBus()
{
    ASSERT_MSG(event_bus_, "Component EventBus reference must be valid");
    return *event_bus_;
}