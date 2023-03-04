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
// const std::string kOneShotDirectory = "/game/resources/audio/oneshot/";
const std::string kOneShotDirectory = "resources/audio/oneshot/";
const std::string kLoopDirectory = "resources/audio/loop/";
const std::string kTestFile = "professionalTestAudio.wav";

AudioService::AudioService()
    : audio_device_(alcOpenDevice(kDefaultDevice)),
      audio_context_(alcCreateContext(audio_device_, 0))
{
}

void AudioService::PlayOneShot(std::string file_name, int gain)
{
    AudioFile<float> audio_file =
        LoadAudioFile(file_name, PlaybackType::ONESHOT);

    // create buffer in memory
    ALuint buffer;
    alGenBuffers(1, &buffer);

    // create source, set to not loop
    ALuint source;
    alGenSources(1, &source);
    alSourcei(source, AL_LOOPING, AL_FALSE);

    // set gain if provided
    if (gain != 0.f)
        alSourcei(source, AL_GAIN, gain);

    // fill buffer
    ALenum file_format = GetFormat(audio_file);
    std::vector<float> file_data = GetData(audio_file);
    size_t file_size = file_data.size();
    ALsizei sample_rate = audio_file.getSampleRate();
    alBufferData(buffer, file_format, file_data.data(), file_size, sample_rate);

    alSourcePlay(source);

    // don't destroy source until done playing
    ALint state = AL_PLAYING;
    while (state == AL_PLAYING)
    {
        Log::debug("AUDIO PLAYING");
        alGetSourcei(source, AL_SOURCE_STATE, &state);
    }
    Log::debug("AUDIO ENDED");
    
    // kill the source when done
    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);
}

// need to implement streaming audio
// instead buffering for longer files (i.e music, etc.)
void AudioService::PlayLoop(std::string file_name, int gain)
{
    AudioFile<float> audio_file = LoadAudioFile(file_name, PlaybackType::LOOP);

    // create buffer in memory
    ALuint buffer;
    alGenBuffers(1, &buffer);

    // create source, set to to loop
    ALuint source;
    alGenSources(1, &source);
    alSourcei(source, AL_LOOPING, AL_TRUE);

    // fill buffer
    ALenum file_format = GetFormat(audio_file);
    std::vector<float> file_data = GetData(audio_file);
    size_t file_size = file_data.size();
    int sample_rate = audio_file.getSampleRate();
    alBufferData(buffer, file_format, file_data.data(), file_size, sample_rate);
    alSourcePlay(source);
}

void AudioService::StopAll()
{
}

/* ----- getters / setters / helpers ----- */

AudioFile<float> AudioService::LoadAudioFile(std::string file_name,
                                             PlaybackType audio_type)
{
    std::string file_path;
    if (audio_type == PlaybackType::LOOP)
        file_path = kLoopDirectory;
    else
        file_path = kOneShotDirectory;
    file_path.append(file_name);

    AudioFile<float> audio_file;
    if (audio_file.load(file_path))
        Log::debug("[AudioService] {} successfully loaded!", file_path);

    return audio_file;
}

ALenum AudioService::GetFormat(AudioFile<float> audio_file)
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

std::vector<float> AudioService::GetData(AudioFile<float> audio_file)
{
    int number_of_channels = audio_file.getNumChannels();
    int samples_per_channel = audio_file.getNumSamplesPerChannel();

    std::vector<float> data;

    for (int channel = 0; channel < number_of_channels; channel++)
    {
        for (int sample = 0; sample < samples_per_channel; sample++)
        {
            data.push_back(audio_file.samples[channel][sample]);
        }
    }

    return data;
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
