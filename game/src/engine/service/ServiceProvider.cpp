#include "engine/service/ServiceProvider.h"

#include "engine/App.h"
#include "engine/core/debug/Log.h"

void ServiceProvider::DispatchInit(Window& window, EventBus& event_bus)
{
    Log::debug("[ServiceProvider] Initializing services");

    ServiceInitializer initializer{
        .window = window, .event_bus = event_bus, .service_provider = *this};

    for (auto& pair : services_)
    {
        pair.second->Init(initializer);
    }
}

void ServiceProvider::DispatchStart()
{
    Log::debug("[ServiceProvider] Starting services");

    for (auto& pair : services_)
    {
        pair.second->OnStart(*this);
    }
}

void ServiceProvider::DispatchUpdate()
{
    for (auto& pair : services_)
    {
        pair.second->OnUpdate();
    }
}

void ServiceProvider::DispatchCleanup()
{
    Log::debug("[ServiceProvider] Cleaning up services");

    for (auto& pair : services_)
    {
        pair.second->OnCleanup();
    }
}