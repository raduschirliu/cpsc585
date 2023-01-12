#include "engine/render/RenderService.h"

#include "engine/core/debug/Log.h"

RenderService::RenderService(Window& window) : renderer_(window)
{
}

void RenderService::SayHi()
{
    Log::info("RenderService: Hiii");
}

void RenderService::Init()
{
    Log::info("RenderService - Initializing");
}

void RenderService::Start()
{
}

void RenderService::Update()
{
}

void RenderService::Cleanup()
{
    Log::info("RenderService - Cleaning up");
}

std::string_view RenderService::GetName() const
{
    return "RenderService";
}
