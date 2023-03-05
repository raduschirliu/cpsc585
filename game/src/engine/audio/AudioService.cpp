#include "AudioService.h"

#include <AL/al.h>
#include <AL/alc.h>

#include <iostream>
#include <iterator>
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
// note: since diagetic sound (3d sound) can only be in mono
// we'll want all sfx to be mono too (music is fine in stereo)
const std::string kSfxDirectory = "resources/audio/sfx/";
const std::string kMusicDirectory = "resources/audio/music/";

// for debugging
const std::string kTestFileName = "professional_test_audio.ogg";

void AudioService::PlayOneShot(std::string file_name, float gain)
{
    // check if source already exists;
    // don't want to keep adding buffers for the same sound
    if (active_sources_.count(file_name) == 0)
        AddSource(file_name);

    ALuint source;
    source = active_sources_[file_name].first;

    // set gain if provided (defaults to 1.f)
    alSourcef(source, AL_GAIN, gain);

    Log::debug("Playing audio: {}", file_name);

    alSourcePlay(source);
}

void AudioService::PlayLoop(std::string file_name, float gain)
{
    // TODO
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
        file_path = kMusicDirectory;
    else
        file_path = kSfxDirectory;
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

    // initialise buffer for audio data
    int number_of_samples =
        audio_file.samples_per_channel * audio_file.number_of_channels_;
    int size_in_bytes = number_of_samples * sizeof(short);

    alBufferData(buffer, audio_file.format_, audio_file.data_.get(),
                 size_in_bytes, audio_file.sample_rate_);

    // create and initialise source
    ALuint source;
    alGenSources(1, &source);
    alSourcei(source, AL_LOOPING, AL_FALSE);
    alSourcei(source, AL_BUFFER, buffer);  // GIVE SOURCE ITS BUFFER

    if (alGetError() != AL_NO_ERROR)
        Log::warning("Couldn't buffer audio data.");

    active_sources_.insert({file_name, {source, buffer}});
}

bool AudioService::IsPlaying(std::string file_name)
{
    ALuint source = active_sources_[file_name].first;
    ALint source_state;
    alGetSourcei(source, AL_SOURCE_STATE, &source_state);

    if (alGetError() != AL_NO_ERROR)
        Log::warning("Couldn't get Source State from {}.", file_name);

    if (source_state != AL_PLAYING)
        return false;

    return true;
}

void AudioService::CullSources()
{
    for (auto pair = active_sources_.begin(),
              next_pair = pair;          //
         pair != active_sources_.end();  //
         pair = next_pair)
    {
        next_pair++;
        if (!IsPlaying(pair->first))
        {
            Log::debug("{} stopped playing.", pair->first);
            alDeleteSources(1, &pair->second.first);
            alDeleteBuffers(1, &pair->second.second);
            
            if (alGetError() != AL_NO_ERROR)
                Log::error("While culling Sources for {}.", pair->first);
           
            active_sources_.erase(pair);
        }
    }
}

/* ----- from Service ------ */

void AudioService::OnInit()
{
    // open and verify audio device
    audio_device_ = alcOpenDevice(kDefaultDevice);
    if (audio_device_)
    {
        Log::debug("[AudioService] opened audio device successfully!");
    }
    else
    {
        Log::warning("[AudioService] couldn't open audio device.");
    }

    // create audio context
    audio_context_ = alcCreateContext(audio_device_, nullptr);
    alcMakeContextCurrent(audio_context_);

    if (alGetError() != AL_NO_ERROR)
    {
        // i.e no audio at all
        Log::warning("Coudn't make audio context current.");
    }
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
    // temp implementation to test audio
    if (input_service_->IsKeyPressed(GLFW_KEY_P))
    {
        PlayOneShot(kTestFileName);
    }

    CullSources();
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
