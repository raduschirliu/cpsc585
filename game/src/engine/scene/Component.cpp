#include "engine/scene/Component.h"

#include "engine/scene/Entity.h"

Component::Component() : event_bus_(nullptr)
{
}

void Component::DispatchInit(ComponentInitializer& initializer)
{
    event_bus_ = &initializer.event_bus;
    entity_ = &initializer.entity;

    Init(initializer.service_provider);
}

Entity& Component::GetEntity() const
{
    ASSERT_MSG(entity_, "Component Entity reference must be valid");
    return *entity_;
}

void Component::Start()
{
}

void Component::Cleanup()
{
}

EventBus& Component::GetEventBus()
{
    ASSERT_MSG(event_bus_, "Component EventBus reference must be valid");
    return *event_bus_;
}