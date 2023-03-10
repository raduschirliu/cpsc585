#include "engine/scene/Component.h"

#include "engine/scene/Entity.h"

static uint32_t kNextId = 0;

Component::Component()
    : id_(kNextId),
      entity_(nullptr),
      event_bus_(nullptr),
      event_sub_ids_{}
{
    kNextId++;
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

void Component::OnDestroy()
{
    for (uint32_t sub_id : event_sub_ids_)
    {
        GetEventBus().Unsubscribe(sub_id);
    }
}

void Component::OnTriggerEnter(const OnTriggerEvent& data)
{
}

void Component::OnTriggerExit(const OnTriggerEvent& data)
{
}

void Component::OnDebugGui()
{
}

EventBus& Component::GetEventBus()
{
    ASSERT_MSG(event_bus_, "Component EventBus reference must be valid");
    return *event_bus_;
}

const uint32_t& Component::GetId() const
{
    return id_;
}

void Component::ManageEventSub(uint32_t subscription_id)
{
    event_sub_ids_.push_back(subscription_id);
}