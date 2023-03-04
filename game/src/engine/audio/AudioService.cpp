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
const std::string kOneShotDirectory = "game/resources/audio/oneshot/";
const std::string kLoopDirectory = "game/resources/audio/loop/";
const std::string kTestFile = "professional_test_audio.wav";

AudioService::AudioService()
    : audio_device_(alcOpenDevice(kDefaultDevice)),
      audio_context_(alcCreateContext(audio_device_, 0)),
{
}

void AudioService::PlayOneShot(std::string file_name, float gain = 0.f)
{
    AudioFile<float> audio_file =
        LoadAudioFile(file_name, PlaybackType::ONESHOT);

    // create buffer in memory
    ALuint buffer;
    alGenBuffers(1, &buffer);

    // create source, set to not loop
    ALuint source;
    alGenSources(1, &source);
    alSourcei(sourcel AL_LOOPING, AL_FALSE);

    // fill buffer
    ALenum format = GetFormat(audio_file);
    float data = GetData(audio_file);
    int file_size = audio_file.size();
    int sample_rate = audio_file.getSampleRate();
    alBufferData(buffer_, format, data, file_size, sample_rate);

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

// need to implement streaming audio
// instead buffering for longer files (i.e music, etc.)
void AudioService::PlayLoop(std::string file_path, float gain = 0.f)
{
    AudioFile<float> audio_file = LoadAudioFile(file_name, PlaybackType::LOOP);

    // create buffer in memory
    ALuint buffer;
    alGenBuffers(1, &buffer);

    // create source, set to to loop
    ALuint source;
    alGenSources(1, &source);
    alSourcei(sourcel AL_LOOPING, AL_TRUE);

    // fill buffer
    ALenum format = GetFormat(audio_file);
    float data = GetData(audio_file);
    int file_size = audio_file.size();
    int sample_rate = audio_file.getSampleRate();
    alBufferData(buffer_, format, data, file_size, sample_rate);

    alSourcePlay(source);
}

void AudioService::StopAll()
{
}

/* ----- getters / setters / helpers ----- */

AudioFile<float> AudioService::LoadAudioFile(std::string file_name,
                                             PlaybackType audio_type)
{
    if (audio_type == PlaybackType::LOOP)
        std::string file_path = kLoopDirectory.append(file_name);
    else
        std::string file_path = kOneShotDirectory.append(file_name);

    AudioFile<float> audio_file;
    audio_file.load(file_path);
    return audio_file;
}

ALenum AudioService::GetFormat(AudioFile audio_file)
{
    if (audio_file.isStereo())
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
    else /* if (audio_file.isMono()) */
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
}

float AudioService::GetData(AudioFile audio_file, float gain_adjust = 0.f)
{
    int amount_of_channels = audio_file.getNumChannels();
    int amount_of_samples = audio_file.getNumSamplesPerChannel();

    float gain = 1.f;

    // iterate through entire file data
    float samples;
    for (int i = 0; i < amount_of_channels; i++)
    {
        for (int j = 0; j < amount_of_samples; j++)
        {
            audio_file.samples[0][i] * (gain + gain_adjust);
        }
    }

    return samples;
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
        audio_file.print(summary);
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
        PlayOneShot(kTestFile);
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
