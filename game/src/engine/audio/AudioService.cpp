#include "AudioService.h"

#include <AL/alc.h>

#include "engine/core/debug/Log.h"

// nullptr means the system's default output device will be used
const ALCchar* default_device = nullptr;

AudioService::AudioService() : alc_device_(alcOpenDevice(default_device))
{
}

void AudioService::OnInit()
{
    if (!alc_device_)
        Log::warning("[AudioService] couldn't open audio device.");
    else
        Log::debug("[AudioService] opened audio device successfully!");
}

void AudioService::OnStart(ServiceProvider& service_provider)
{
}

void AudioService::OnSceneLoaded(Scene& scene)
{
}

void AudioService::OnUpdate()
{
}

void AudioService::OnCleanup()
{
    alcCloseDevice(alc_device_);
}

std::string_view AudioService::GetName() const
{
    return "AudioService";
}
