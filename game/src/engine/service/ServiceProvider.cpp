#include "engine/service/ServiceProvider.h"

#include "engine/core/debug/Log.h"

void ServiceProvider::DispatchInit()
{
    Log::debug("[ServiceProvider] Initializing services");

    for (auto& pair : services_)
    {
        pair.second->OnInit();
    }
}

void ServiceProvider::DispatchStart()
{
    Log::debug("[ServiceProvider] Starting services");

    for (auto& pair : services_)
    {
        pair.second->OnStart();
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