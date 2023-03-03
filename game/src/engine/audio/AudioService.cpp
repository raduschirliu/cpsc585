#include "AudioService.h"

#include <AL/alc.h>
#include <AudioFile.h>

#include <object_ptr.hpp>

#include "engine/core/debug/Log.h"
#include "engine/input/InputService.h"
#include "engine/service/Service.h"
#include "engine/service/ServiceProvider.h"

// the system's default output device will be used
const ALCchar* default_device = nullptr;
ALCint context_attributes = 0;

AudioService::AudioService()
    : audio_device_(alcOpenDevice(default_device)),
      audio_context_(alcCreateContext(audio_device_, 0))
{
}

void AudioService::OnInit()
{
    // check audio device
    if (!audio_device_)
    {
        Log::warning("[AudioService] couldn't open audio device.");
    }
    else
    {
        Log::debug("[AudioService] opened audio device successfully!");
    }

    alcMakeContextCurrent(audio_context_);
}

void AudioService::OnStart(ServiceProvider& service_provider)
{
    input_service_ = &service_provider.GetService<InputService>();
}

void AudioService::OnSceneLoaded(Scene& scene)
{
}

void AudioService::OnUpdate()
{
    // test audio
    if (input_service_->IsKeyPressed(GLFW_KEY_SPACE))
    {
        /* todo: play sound */ Log::debug("SPACE was pressed.");
    }
}

void AudioService::OnCleanup()
{
    alcMakeContextCurrent(nullptr);  // clear context
    alcDestroyContext(audio_context_);
    alcCloseDevice(audio_device_);
}

std::string_view AudioService::GetName() const
{
    return "AudioService";
}
