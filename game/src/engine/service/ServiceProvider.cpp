#include "engine/service/ServiceProvider.h"

#include "engine/App.h"
#include "engine/core/debug/Log.h"

void ServiceProvider::DispatchInit(Window& window)
{
    Log::debug("[ServiceProvider] Initializing services");

    ServiceInitializer initializer{.window = window, .service_provider = *this};

    for (auto& pair : services_)
    {
        pair.service->Init(initializer);
    }
}

void ServiceProvider::DispatchSceneChange(Scene& scene)
{
    Log::debug("[ServiceProvider] Changing scene");

    for (auto& pair: services_)
    {
        pair.service->UpdateActiveScene(scene);
    }
}

void ServiceProvider::DispatchStart()
{
    Log::debug("[ServiceProvider] Starting services");

    for (auto& pair : services_)
    {
        pair.service->OnStart(*this);
    }
}

void ServiceProvider::DispatchUpdate()
{
    for (auto& pair : services_)
    {
        pair.service->OnUpdate();
    }
}

void ServiceProvider::DispatchCleanup()
{
    Log::debug("[ServiceProvider] Cleaning up services");

    for (auto& pair : services_)
    {
        pair.service->OnCleanup();
    }
}