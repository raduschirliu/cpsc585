#include "engine/service/Service.h"

#include "engine/App.h"
#include "engine/scene/Scene.h"
#include "engine/service/ServiceProvider.h"

void Service::Init(ServiceInitializer& initializer)
{
    window_ = &initializer.window;
    app_ = &initializer.app;
    event_bus_ = &app_->GetEventBus();

    OnInit();
}

void Service::OnSceneLoaded(Scene& scene)
{
    // To be overridden
}

void Service::OnSceneUnloaded(Scene& scene)
{
    // To be overridden
}

void Service::OnWindowSizeChanged(int width, int height)
{
    // To be overridden
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

App& Service::GetApp()
{
    ASSERT_MSG(app_, "Service must have valid App reference");
    return *app_;
}