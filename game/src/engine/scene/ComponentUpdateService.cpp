#include "engine/scene/ComponentUpdateService.h"

#include "engine/core/debug/Log.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/service/ServiceProvider.h"

using std::string_view;

void ComponentUpdateService::OnInit()
{
}

void ComponentUpdateService::OnStart(ServiceProvider& service_provider)
{
}

void ComponentUpdateService::OnUpdate()
{
    GetEventBus().Publish<OnUpdateEvent>();
}

void ComponentUpdateService::OnCleanup()
{
}

string_view ComponentUpdateService::GetName() const
{
    return "ComponentUpdateService";
}