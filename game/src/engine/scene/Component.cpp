#include "engine/scene/Component.h"

#include "engine/scene/Entity.h"

Component::Component() : entity_(nullptr), event_bus_(nullptr)
{
}

void Component::Init(ComponentInitializer& initializer)
{
    event_bus_ = &initializer.event_bus;
    entity_ = &initializer.entity;

    OnInit(initializer.service_provider);
}

Entity& Component::GetEntity()
{
    ASSERT_MSG(entity_, "Component Entity reference must be valid");
    return *entity_;
}

void Component::OnStart()
{
}

void Component::OnCleanup()
{
}

EventBus& Component::GetEventBus()
{
    ASSERT_MSG(event_bus_, "Component EventBus reference must be valid");
    return *event_bus_;
}