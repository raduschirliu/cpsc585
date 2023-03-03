#include "AudioService.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <AudioFile.h>

#include <object_ptr.hpp>
#include <string>

#include "engine/core/debug/Log.h"
#include "engine/input/InputService.h"
#include "engine/service/Service.h"
#include "engine/service/ServiceProvider.h"

// the system's default output device will be used
const ALCchar* kDefaultDevice = nullptr;
ALCint kContextAttributes = 0;

// audio directories
const std::string kSfxDirectory = "game/resources/audio/sfx/";
const std::string kMusicDirectory = "game/resources/audio/music/";
const std::string kTestFile = "professional_test_audio.wav";

AudioService::AudioService()
    : audio_device_(alcOpenDevice(kDefaultDevice)),
      audio_context_(alcCreateContext(audio_device_, 0)),
      buffer_()
{
}

void AudioService::PlayOneShot(std::string file_name)
{
    // load audio file
    std::string file_path = kSfxDirectory.append(file_name);
    AudioFile<float> audio_file;
    audio_file.load(file_path);

    // create source, set to to not loop
    ALuint source;
    alGenSources(1, &source);
    alSourcei(sourcel AL_LOOPING, AL_FALSE);

    // load file, add to buffer, play to source
    audio_file.load(file_name);
    alBufferData(buffer_, GetAudioFileFormat(audio_file),
                 GetAudioFileData(audio_file), audio_file.size(),
                 audio_file.getSampleRate());
    alSourcePlay(source);

    // don't destroy source until done playing
    ALint state = AL_PLAYING;
    while (state == AL_PLAYING)
    {
        alGetSourcei(source, AL_SOURCE_STATE, &state);
    }

    // kill the source when done
    alDeleteSources(1, &source);
}

/* ----- getters / setters / helpers ----- */

ALenum AudioService::GetAudioFileFormat(AudioFile audio_file)
{
    if (audio_file.isMono()
    {
        if (audio_file.getBitDepth() == 8)
        {
            return AL_FORMAT_MONO8;
        }
        else if (audio_file.getBitDepth() == 16)
        {
            return AL_FORMAT_MONO16;
        }
    }
    else if (audio_file.isStereo())
    {
        if (audio_file.getBitDepth() == 8)
        {
            return AL_FORMAT_STEREO8;
        }
        else if (audio_file.getBitDepth() == 16)
        {
            return AL_FORMAT_STEREO16;
        }
    }
}

double AudioService::GetAudioFileData(AudioFile audio_file)
{
    int numSamples = audio_file.getNumSamplesPerChannel();
    double samples;

    // iterate through each sample's data
    for (int i = 0; i < numSamples; i++)
    {
        samples += audio_file.samples[0][i];
    }

    return samples;
}

/* ----- from service ------ */

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
        audio_file.print(summary);
    }

    alcMakeContextCurrent(audio_context_);
    alGenBuffers(1, &buffer_)
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
        PlayOneShot(kTestFile);
    }
}

void AudioService::OnCleanup()
{
    alDeleteBuffers(1, buffer_);
    alcMakeContextCurrent(nullptr);  // clear context
    alcDestroyContext(audio_context_);
    alcCloseDevice(audio_device_);
}

std::string_view AudioService::GetName() const
{
    return "AudioService";
}