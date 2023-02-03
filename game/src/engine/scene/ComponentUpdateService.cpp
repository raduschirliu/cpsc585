#include "engine/scene/ComponentUpdateService.h"

#include <GLFW/glfw3.h>

#include <chrono>

#include "engine/core/debug/Log.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/service/ServiceProvider.h"

using std::make_unique;
using std::string_view;

ComponentUpdateService::ComponentUpdateService() : last_frame_()
{
}

void ComponentUpdateService::OnInit()
{
}

void ComponentUpdateService::OnStart(ServiceProvider& service_provider)
{
}

void ComponentUpdateService::OnUpdate()
{
    const Timestep current_frame = Timestep::Seconds(glfwGetTime());
    const Timestep delta_time = current_frame - last_frame_;
    last_frame_ = current_frame;

    auto event_data = make_unique<OnUpdateEvent>();
    event_data->delta_time = delta_time;

    GetEventBus().Publish<OnUpdateEvent>(event_data.get());
}

void ComponentUpdateService::OnCleanup()
{
}

string_view ComponentUpdateService::GetName() const
{
    return "ComponentUpdateService";
}