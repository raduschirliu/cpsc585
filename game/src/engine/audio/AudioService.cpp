#include "AudioService.h"

#include <AL/al.h>
#include <AL/alc.h>

#include <iostream>
#include <memory>
#include <object_ptr.hpp>
#include <string>
#include <utility>

#include "AudioFile.h"
#include "engine/core/debug/Log.h"
#include "engine/input/InputService.h"
#include "engine/service/Service.h"
#include "engine/service/ServiceProvider.h"
#include "stb/stb_vorbis.c"

// the system's default output device will be used
const ALCchar* kDefaultDevice = nullptr;
ALCint kContextAttributes = 0;

// audio directories
const std::string kOneShotDirectory = "resources/audio/sfx/";
const std::string kLoopDirectory = "resources/audio/music/";

// debugging
const std::string kTestFileName = "TestLaugh_44k.ogg";

AudioService::AudioService() : audio_device_(alcOpenDevice(kDefaultDevice))
{
}

void AudioService::PlayOneShot(std::string file_name, float gain)
{
    AddSource(file_name);
    ALuint source = active_sources_[file_name].first;

    // set gain if provided
    alSourcef(source, AL_GAIN, gain);

    alSourcePlay(source);
}

/// @todo implement streaming audio
/// instead buffering for longer files (i.e music, etc.)
void AudioService::PlayLoop(std::string file_name, float gain)
{
}

void AudioService::StopPlayback(std::string file_name)
{
    ALuint source = active_sources_[file_name].first;

    alSourceStop(source);
}

void AudioService::StopAllPlayback()
{
    for (auto& pair : active_sources_)
    {
        alSourceStop(pair.second.first);
    }
}

/* ----- getters / setters / helpers ----- */

AudioFile AudioService::LoadAudioFile(std::string file_name, bool is_looping)
{
    std::string file_path;

    if (is_looping)
        file_path = kLoopDirectory;
    else
        file_path = kOneShotDirectory;
    file_path.append(file_name);

    // initialize AudioFile
    AudioFile audio_file;
    short* file_data;

    audio_file.samples_per_channel = stb_vorbis_decode_filename(
        file_path.c_str(), &audio_file.number_of_channels_,
        &audio_file.sample_rate_, &file_data);

    audio_file.data_ = std::unique_ptr<short>(file_data);

    Log::debug("Succesfully opened audio file: {}", file_name);
    Log::debug(
        "File properties:\n"
        "\t\tnum of channels: {}\n"
        "\t\tnum of samples: {}\n"
        "\t\tsample rate: {}\n",
        audio_file.number_of_channels_, audio_file.samples_per_channel,
        audio_file.sample_rate_);

    if (audio_file.number_of_channels_ == 2)
        audio_file.format_ = AL_FORMAT_STEREO16;
    else
        audio_file.format_ = AL_FORMAT_MONO16;

    return audio_file;
}

void AudioService::AddSource(std::string file_name, bool is_looping)
{
    AudioFile audio_file = LoadAudioFile(file_name, false);

    // create buffer in memory
    ALuint buffer;
    alGenBuffers(1, &buffer);

    // create source, set to not loop
    ALuint source;
    alGenSources(1, &source);
    alSourcei(source, AL_LOOPING, AL_FALSE);

    // fill buffer
    int number_of_samples =
        audio_file.samples_per_channel * audio_file.number_of_channels_;
    int size_in_bytes = number_of_samples * sizeof(short);

    alBufferData(buffer, audio_file.format_, audio_file.data_.get(),
                 size_in_bytes, audio_file.sample_rate_);

    if (alGetError() != AL_NO_ERROR)
        Log::warning("Couldn't buffer audio data.");

    active_sources_.insert({file_name, {source, buffer}});
}

/* ----- from Service ------ */

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

    audio_context_ = alcCreateContext(audio_device_, nullptr);
    alcMakeContextCurrent(audio_context_);
    if (alGetError() != AL_NO_ERROR)
        Log::warning("Coudn't make audio context current.");
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
    if (input_service_->IsKeyPressed(GLFW_KEY_P))
    {
        PlayOneShot(kTestFileName);
        ALuint source = active_sources_[kTestFileName].first;
        ALint source_state = AL_PLAYING;
        if (source_state == AL_PLAYING)
        {
            alGetSourcei(source, AL_SOURCE_STATE, &source_state);
            Log::debug("Audio playing.");
        }
        Log::debug("Audio ended.");
    }
}

void AudioService::OnCleanup()
{
    for (auto& source : active_sources_)
    {
        alDeleteSources(1, &source.second.first);
        alDeleteBuffers(1, &source.second.second);
    }
    alcMakeContextCurrent(nullptr);  // clear context
    alcDestroyContext(audio_context_);
    alcCloseDevice(audio_device_);
}

std::string_view AudioService::GetName() const
{
    return "AudioService";
}
