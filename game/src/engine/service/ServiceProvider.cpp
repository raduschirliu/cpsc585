#include "engine/service/ServiceProvider.h"

#include "engine/App.h"
#include "engine/core/debug/Log.h"

void ServiceProvider::DispatchInit(App& app)
{
    Log::debug("[ServiceProvider] Initializing services");

    ServiceInitializer initializer{
        .window = app.GetWindow(), .service_provider = *this, .app = app};

    for (auto& pair : services_)
    {
        pair.service->Init(initializer);
    }
}

void ServiceProvider::DispatchSceneLoaded(Scene& scene)
{
    for (auto& pair : services_)
    {
        pair.service->OnSceneLoaded(scene);
    }
}

void ServiceProvider::DispatchSceneUnloaded(Scene& scene)
{
    for (auto& pair : services_)
    {
        pair.service->OnSceneUnloaded(scene);
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