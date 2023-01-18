#include "engine/render/RenderService.h"

#include "engine/core/debug/Log.h"

RenderService::RenderService() : renderer_()
{
}

void RenderService::SayHi()
{
    Log::info("RenderService: Hiii");
}

void RenderService::OnInit()
{
    Log::info("RenderService - Initializing");
    renderer_.Init();
}

void RenderService::OnStart(ServiceProvider& service_provider)
{
}

void RenderService::OnUpdate()
{
    renderer_.RenderFrame();
}

void RenderService::OnCleanup()
{
    Log::info("RenderService - Cleaning up");
    renderer_.Cleanup();
}

std::string_view RenderService::GetName() const
{
    return "RenderService";
}
