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

// the current music file to stream from
AudioFile kMusicAudioFile;
// number of buffers to use when streaming music files
const std::size_t kStreamBufferAmount = 4;
const std::size_t kStreamBufferSize = 65536;  // per buffer
// just keeps track of how much of the file was played
std::size_t kMusicPlayhead = kStreamBufferSize * kStreamBufferAmount;

// for debugging
const std::string kTestFileName = "professional_test_audio.ogg";
const std::string kTestMusic = "professional_test_music.ogg";

/* ----- handling playback ----- */

void AudioService::PlaySource(std::string file_name, bool is_looping)
{
    // check if source already exists;
    // don't want to keep adding buffers for the same sound
    if (!SourceExists(file_name))
    {
        AddSource(file_name);
    }

    ALuint source;
    source = active_sources_[file_name].first;

    if (is_looping)
    {
        alSourcef(source, AL_LOOPING, AL_TRUE);
    }
    else
    {
        alSourcef(source, AL_LOOPING, AL_FALSE);
    }

    Log::debug("Playing audio: {}", file_name);

    alSourcePlay(source);
}

void AudioService::PlayMusic(std::string file_name, float gain)
{
    //  music wasn't set yet
    if (music_source_.first == "")
    {
        SetMusic(file_name);
    }

    ALuint source;
    source = music_source_.second.first;

    alSourcef(source, AL_GAIN, gain);

    Log::debug("Starting playing music: {}", file_name);

    alSourcePlay(source);
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

void AudioService::SetPitch(std::string file_name, float pitch_offset)
{
    if (!SourceExists(file_name))
    {
        Log::error("Couldn't open or find file: {}", file_name);
        return;
    }

    ALuint source;
    source = active_sources_[file_name].first;

    alSourcef(source, AL_PITCH, pitch_offset);
}

void AudioService::SetGain(std::string file_name, float gain)
{
    if (!SourceExists(file_name))
    {
        Log::error("Couldn't open or find file: {}", file_name);
        return;
    }

    ALuint source;
    source = active_sources_[file_name].first;

    alSourcef(source, AL_GAIN, gain);
}

void AudioService::SetLooping(std::string file_name, bool is_looping)
{
    if (!SourceExists(file_name))
    {
        Log::error("Couldn't open or find file: {}", file_name);
        return;
    }
}

AudioFile AudioService::LoadAudioFile(std::string file_name, bool is_music)
{
    std::string file_path;

    if (is_music)
    {
        file_path = kMusicDirectory;
    }
    else
    {
        file_path = kSfxDirectory;
    }
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
    {
        audio_file.format_ = AL_FORMAT_STEREO16;
    }
    else
    {
        audio_file.format_ = AL_FORMAT_MONO16;
    }

    return audio_file;
}

void AudioService::SetMusic(std::string file_name)
{
    kMusicAudioFile = LoadAudioFile(file_name, true);

    // reserve buffers in memory
    ALuint buffers[kStreamBufferAmount];
    alGenBuffers(kStreamBufferAmount, &buffers[0]);

    // initialise buffers for audio data
    auto audio_data = kMusicAudioFile.data_.get();

    for (std::size_t i = 0; i < kStreamBufferAmount; ++i)
    {
        alBufferData(buffers[i], kMusicAudioFile.format_,
                     &audio_data[i * kStreamBufferSize], kStreamBufferSize,
                     kMusicAudioFile.sample_rate_);

        if (alGetError() != AL_NO_ERROR)
        {
            Log::error("Couldn't buffer audio data.");
        }
    }

    // create and initialise source
    ALuint source;
    alGenSources(1, &source);
    alSourcei(source, AL_LOOPING, AL_FALSE);

    // queue buffers for source
    alSourceQueueBuffers(source, kStreamBufferAmount, &buffers[0]);

    if (alGetError() != AL_NO_ERROR)
    {
        Log::error("Couldn't queue audio buffers.");
    }

    music_source_ = {file_name, {source, buffers}};
}

void AudioService::AddSource(std::string file_name)
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
    {
        Log::warning("Couldn't buffer audio data.");
    }

    active_sources_.insert({file_name, {source, buffer}});
}

bool AudioService::IsPlaying(std::string file_name)
{
    ALuint source = active_sources_[file_name].first;
    ALint source_state;
    alGetSourcei(source, AL_SOURCE_STATE, &source_state);

    if (alGetError() != AL_NO_ERROR)
    {
        Log::warning("Couldn't get Source State from {}.", file_name);
    }

    if (source_state != AL_PLAYING)
    {
        return false;
    }

    return true;
}

void AudioService::CullSources()
{
    // iterate through active sources
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
            {
                Log::error("While culling Sources for {}.", pair->first);
            }

            active_sources_.erase(pair);
        }
    }
}

bool AudioService::SourceExists(std::string file_name)
{
    if (active_sources_.count(file_name) == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void AudioService::UpdateStreamBuffer()
{
    ALuint source = music_source_.second.first;
    ALuint* buffers = music_source_.second.second;

    ALint buffers_processed = 0;
    alGetSourcei(source, AL_BUFFERS_PROCESSED, &buffers_processed);

    // we haven't streamed a buffer yet
    if (buffers_processed <= 0)
    {
        return;
    }
    int number_of_samples = kMusicAudioFile.samples_per_channel *
                            kMusicAudioFile.number_of_channels_;
    int size_in_bytes = number_of_samples * sizeof(short);

    auto audio_data = kMusicAudioFile.data_.get();

    Log::debug("buffers processed: {}", buffers_processed);
    Log::debug("playhead at: {}/{}", kMusicPlayhead, size_in_bytes);
    // for every buffer in queue that was played
    while (buffers_processed > 0)
    {
        // pop off already played buffer
        ALuint buffer;
        alSourceUnqueueBuffers(source, 1, &buffer);
        buffers_processed--;

        // reserve memory for new audio data
        short* new_data = new short[kStreamBufferSize];
        std::memset(new_data, 0, kStreamBufferSize);

        std::size_t new_data_size_bytes = kStreamBufferSize;

        // for when the remainder of the file is less than a buffer size
        // if (kMusicPlayhead + kStreamBufferSize > size_in_bytes)
        // {
        //     new_data_size_bytes = size_in_bytes - kMusicPlayhead;
        // }

        // get new data
        std::memcpy(&new_data[0], &audio_data[kMusicPlayhead],
                    new_data_size_bytes);

        // advance playhead
        kMusicPlayhead += new_data_size_bytes;

        // when remainder is less than buffer size, fill buffer with the
        // beginning of the file for a seamless loop
        // if (new_data_size_bytes < kStreamBufferSize)
        // {
        //     // loop back to beginning of song
        //     kMusicPlayhead = 0;
        //     int buffer_size = kStreamBufferSize - new_data_size_bytes;
        //     std::memcpy(&new_data[new_data_size_bytes],
        //                 &audio_data[kMusicPlayhead], buffer_size);
        // }

        // copy new data into buffer and queue it
        alBufferData(buffer, kMusicAudioFile.format_, new_data,
                     kStreamBufferSize, kMusicAudioFile.sample_rate_);
        alSourceQueueBuffers(source, 1, &buffer);
        if (alGetError() != AL_NO_ERROR)
            Log::error("Couldn't stream audio.");

        // clean up
        delete[] new_data;
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
    // ex. implementation to test audio
    if (input_service_->IsKeyPressed(GLFW_KEY_P))
    {
        // AddSource(kTestFileName);
        // PlaySource(kTestFileName);
        SetMusic(kTestMusic);
        PlayMusic(kTestMusic, 0.5f);
    }
    CullSources();
    if (music_source_.second.first != NULL)
        UpdateStreamBuffer();
}

void AudioService::OnCleanup()
{
    // clear music
    alDeleteSources(1, &music_source_.second.first);
    alDeleteBuffers(kStreamBufferAmount, music_source_.second.second);

    // clear sfx
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
