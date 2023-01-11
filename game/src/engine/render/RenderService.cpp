#include "engine/render/RenderService.h"

#include "engine/core/debug/Log.h"

void RenderService::SayHi()
{
    Log::info("RenderService: Hiii");
}

void RenderService::Init()
{
}

void RenderService::Cleanup()
{
}

std::string_view RenderService::GetName() const
{
    return "RenderService";
}