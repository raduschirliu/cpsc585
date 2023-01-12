#include "engine/service/ServiceProvider.h"

void ServiceProvider::OnUpdate()
{
    for (auto& pair : services_)
    {
        pair.second->Update();
    }
}

void ServiceProvider::OnCleanup()
{
    for (auto& pair : services_)
    {
        pair.second->Cleanup();
    }
}