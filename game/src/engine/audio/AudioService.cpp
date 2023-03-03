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

const std::string kTestFilePath =
    "game/resources/audio/sfx/professional_test_audio.wav";

AudioService::AudioService()
    : audio_device_(alcOpenDevice(kDefaultDevice)),
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
        audio_file_.print(summary);
    }

    alcMakeContextCurrent(audio_context_);

    if (audio_file_.load(kTestFilePath))
    {
        Log::warning("[AudioService] couldn't open audio file :<");
    }
    else
    {
        Log::debug("[AudioService] opened audio file without bresking!");
    }

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
        PlayOneShot(kTestFilePath);
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

void AudioService::PlayOneShot(std::string file_name)
{
    // create source
    ALuint source;
    alGenSources(1, &source);
    // do not loop
    alSourcei(sourcel AL_LOOPING, AL_FALSE);

    // load file, add to buffer, play to source
    audio_file_.load(file_name);
    alBufferData(buffer_, GetAudioFileFormat(audio_file_),
                 GetAudioFileData(audio_file_), audio_file_.size(),
                 audio_file_.getSampleRate());
    alSourcePlay(source);

    ALint state = AL_PLAYING;
    while (state == AL_PLAYING)
    {
        alGetSourcei(source, AL_SOURCE_STATE, &state);
    }

    // kill the source when done
    alDeleteSources(1, &source);
}

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

double AudioService::GetAudioFileData(AudioFile audio_path)
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
