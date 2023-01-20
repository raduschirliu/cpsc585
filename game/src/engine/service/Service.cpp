#include "engine/service/Service.h"

#include "engine/service/ServiceProvider.h"

void Service::Init(ServiceInitializer& initializer)
{
    window_ = &initializer.window;
    event_bus_ = &initializer.event_bus;

    OnInit();
}

void Service::OnInit()
{
    // To be overridden
}

void Service::OnStart(ServiceProvider& service_provider)
{
    // To be overridden
}

void Service::OnCleanup()
{
    // To be overridden
}

Window& Service::GetWindow()
{
    ASSERT_MSG(window_, "Service must have valid Window reference");
    return *window_;
}

EventBus& Service::GetEventBus()
{
    ASSERT_MSG(event_bus_, "Service must have valid EventBus reference");
    return *event_bus_;
}