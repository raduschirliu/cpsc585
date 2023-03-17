#include "AudioService.h"

#include <AL/al.h>
#include <AL/alc.h>

#include <glm/glm.hpp>
#include <iostream>
#include <iterator>
#include <memory>
#include <object_ptr.hpp>
#include <string>
#include <utility>

#include "AudioFile.h"
#include "engine/core/debug/Log.h"
#include "engine/input/InputService.h"
#include "engine/scene/Entity.h"
#include "engine/scene/Transform.h"
#include "engine/service/Service.h"
#include "engine/service/ServiceProvider.h"
#include "stb/stb_vorbis.c"

// the system's default output device will be used
static const ALCchar* kDefaultDevice = nullptr;
static ALCint kContextAttributes = 0;

// audio directories
static const std::string kSfxDirectory = "resources/audio/sfx/";
static const std::string kMusicDirectory = "resources/audio/music/";

// number of buffers to use when streaming music files
static constexpr std::size_t kStreamBufferAmount = 4;
static constexpr ALsizei kStreamBufferSize = 65536;  // 32kb per buffer

// for debugging
static const std::string kTestFileName = "test_audio.ogg";
static const std::string kTestMusic = "test_music.ogg";

/* ----- setting sources ----- */

void AudioService::AddSource(std::string file_name)
{
    AudioFile audio_file = LoadAudioFile(file_name, false);

    // create buffer in memory
    ALuint buffer;
    alGenBuffers(1, &buffer);

    // initialise buffer for audio data
    alBufferData(buffer, audio_file.format_, audio_file.data_.get(),
                 audio_file.GetSizeBytes(), audio_file.sample_rate_);

    if (alGetError() != AL_NO_ERROR)
    {
        Log::error("Couldn't buffer audio data for {}.", file_name);
    }

    // create and initialise source
    ALuint source;
    alGenSources(1, &source);
    alSourcei(source, AL_LOOPING, AL_FALSE);
    alSourcei(source, AL_BUFFER, buffer);  // GIVE SOURCE ITS BUFFER

    if (alGetError() != AL_NO_ERROR)
    {
        Log::error("Couldn't create source for {}", file_name);
    }

    non_diegetic_sources_.insert({file_name, {source, buffer}});
}

void AudioService::AddSource(std::string file_name, std::uint32_t entity_id)
{
    AudioFile audio_file = LoadAudioFile(file_name, false);

    // create buffer in memory
    ALuint buffer;
    alGenBuffers(1, &buffer);

    // initialise buffer for audio data
    alBufferData(buffer, audio_file.format_, audio_file.data_.get(),
                 audio_file.GetSizeBytes(), audio_file.sample_rate_);

    if (alGetError() != AL_NO_ERROR)
    {
        Log::error("Couldn't buffer audio data for {}.", file_name);
    }

    // create and initialise source
    ALuint source;
    alGenSources(1, &source);
    alSourcei(source, AL_LOOPING, AL_FALSE);
    alSourcei(source, AL_BUFFER, buffer);  // GIVE SOURCE ITS BUFFER

    // set properties for spatial audio
    alSourcef(source, AL_MAX_DISTANCE, 300.0f);  // distance until silent
    alSourcef(source, AL_ROLLOFF_FACTOR, 0.5f);

    if (alGetError() != AL_NO_ERROR)
    {
        Log::error("Couldn't create source for {}", file_name);
    }

    diegetic_sources_.insert({entity_id, {source, buffer}});
}

void AudioService::SetMusic(std::string file_name)
{
    music_file_ = LoadAudioFile(file_name, true);

    // reserve buffers in memory
    ALuint* buffers = new ALuint[kStreamBufferAmount];
    alGenBuffers(kStreamBufferAmount, &buffers[0]);

    // initialise buffers for audio data
    auto audio_data = music_file_.data_.get();

    for (std::size_t i = 0; i < kStreamBufferAmount; i++)
    {
        alBufferData(buffers[i], music_file_.format_,
                     &audio_data[(i * kStreamBufferSize) / sizeof(short)],
                     kStreamBufferSize, music_file_.sample_rate_);

        if (alGetError() != AL_NO_ERROR)
        {
            Log::error("Couldn't buffer audio data for {}.", file_name);
        }
    }
    playhead_ = kStreamBufferSize * kStreamBufferAmount;

    // create and initialise source
    ALuint source;
    alGenSources(1, &source);
    // don't want to loop just one buffer
    alSourcei(source, AL_LOOPING, AL_FALSE);

    // queue buffers for source
    alSourceQueueBuffers(source, kStreamBufferAmount, &buffers[0]);

    if (alGetError() != AL_NO_ERROR)
    {
        Log::error("Couldn't queue audio buffers for {}.", file_name);
    }

    music_source_ = {file_name, {source, buffers}};
}

/* ----- playback functions ----- */

void AudioService::PlaySource(std::string file_name)
{
    // check if source already exists;
    if (!SourceExists(file_name))
    {
        AddSource(file_name);  // we'll set it here just to be nice
    }

    // find the source
    ALuint source;
    source = non_diegetic_sources_[file_name].first;

    alSourcePlay(source);

    if (alGetError() != AL_NO_ERROR)
    {
        Log::error("Couldn't play audio for {}.", file_name);
        return;
    }

    Log::debug("Playing audio: {}", file_name);
}

void AudioService::PlaySource(std::uint32_t entity_id)
{
    // check if source already exists;
    if (!SourceExists(entity_id))
    {
        Log::warning("Source for entity {} doesn't exist yet.", entity_id);
        return;
    }

    // find the source
    ALuint source;
    source = diegetic_sources_[entity_id].first;
    // alSourcei(source, AL_LOOPING, AL_TRUE); // debugging

    alSourcePlay(source);

    if (alGetError() != AL_NO_ERROR)
    {
        Log::error("Couldn't play audio for entity {}.", entity_id);
        return;
    }

    Log::debug("Entity {} playing audio.", entity_id);

    float x;
    float y;
    float z;

    alGetSource3f(source, AL_POSITION, &x, &y, &z);
    Log::debug("Source position: {}, {}, {}", x, y, z);

    alGetListener3f(AL_POSITION, &x, &y, &z);
    Log::debug("AudioListener position: {}, {}, {}", x, y, z);
}

void AudioService::PlayMusic(std::string file_name)
{
    //  music wasn't set yet
    if (music_source_.first == "")
    {
        SetMusic(file_name);  // we'll set it here just to be nice
    }

    // find the source
    ALuint source;
    source = music_source_.second.first;

    alSourcef(source, AL_GAIN, 0.2f);
    alSourcePlay(source);

    if (alGetError() != AL_NO_ERROR)
    {
        Log::error("Couldn't play music for {}.", file_name);
        return;
    }

    Log::debug("Starting playing music: {}", file_name);
}

void AudioService::StopSource(std::string file_name)
{
    ALuint source = non_diegetic_sources_[file_name].first;
    alSourceStop(source);
}

void AudioService::StopSource(std::uint32_t entity_id)
{
    ALuint source = diegetic_sources_[entity_id].first;
    alSourceStop(source);
}

void AudioService::StopAllSources()
{
    for (auto& pair : non_diegetic_sources_)
    {
        alSourceStop(pair.second.first);
    }
}

void AudioService::StopMusic()
{
    ALuint source = music_source_.second.first;
    alSourceStop(source);
}

/* ----- setters ------ */

void AudioService::SetPitch(std::string file_name, float pitch_offset)
{
    if (!SourceExists(file_name))
    {
        Log::error("Couldn't open or find file: {}", file_name);
        return;
    }

    ALuint source;
    source = non_diegetic_sources_[file_name].first;

    alSourcef(source, AL_PITCH, pitch_offset);

    if (alGetError() != AL_NO_ERROR)
    {
        Log::error("Couldn't set pitch for {}.", file_name);
    }

    Log::debug("Offset pitch of {} by {}", file_name, pitch_offset);
}

void AudioService::SetPitch(std::uint32_t entity_id, float pitch_offset)
{
    if (!SourceExists(entity_id))
    {
        Log::error("Source for entity: {} doesn't exist.", entity_id);
        return;
    }

    ALuint source;
    source = diegetic_sources_[entity_id].first;

    alSourcef(source, AL_PITCH, pitch_offset);

    if (alGetError() != AL_NO_ERROR)
    {
        Log::error("Couldn't set pitch for {}.", entity_id);
    }

    Log::debug("Offset pitch of entity: {} by {}", entity_id, pitch_offset);
}

void AudioService::SetGain(std::string file_name, float gain)
{
    if (!SourceExists(file_name))
    {
        Log::error("Couldn't open or find file: {}", file_name);
        return;
    }

    ALuint source;
    source = non_diegetic_sources_[file_name].first;

    alSourcef(source, AL_GAIN, gain);

    if (alGetError() != AL_NO_ERROR)
    {
        Log::error("Couldn't set gain for {}.", file_name);
    }

    Log::debug("Offset pitch of {} by {}", file_name, gain);
}

void AudioService::SetGain(std::uint32_t entity_id, float gain)
{
    if (!SourceExists(entity_id))
    {
        Log::error("Source for entity: {} doesn't exist.", entity_id);
        return;
    }

    ALuint source;
    source = diegetic_sources_[entity_id].first;

    alSourcef(source, AL_GAIN, gain);

    if (alGetError() != AL_NO_ERROR)
    {
        Log::error("Couldn't set gain for {}.", entity_id);
    }

    Log::debug("Set gain for entity: {} by {}", entity_id, gain);
}

void AudioService::SetLooping(std::string file_name, bool is_looping)
{
    if (!SourceExists(file_name))
    {
        Log::error("Couldn't open or find file: {}", file_name);
        return;
    }

    ALuint source;
    source = non_diegetic_sources_[file_name].first;

    if (is_looping)
    {
        alSourcef(source, AL_LOOPING, AL_TRUE);
        Log::debug("Set {} to loop.", file_name);
    }
    else
    {
        alSourcef(source, AL_LOOPING, AL_FALSE);
        Log::debug("Set {} to not loop.", file_name);
    }
}

void AudioService::SetLooping(std::uint32_t entity_id,  bool is_looping)
{
    if (!SourceExists(entity_id))
    {
        Log::error("Source for entity: {} doesn't exist.", entity_id);
        return;
    }

    ALuint source;
    source = diegetic_sources_[entity_id].first;

    if (is_looping)
    {
        alSourcef(source, AL_LOOPING, AL_TRUE);
        Log::debug("Set source for entity: {} to loop.", entity_id);
    }
    else
    {
        alSourcef(source, AL_LOOPING, AL_FALSE);
        Log::debug("Set source for entity: {} to not loop.", entity_id);
    }
}

void AudioService::SetSourcePosition(std::uint32_t entity_id,
                                     glm::vec3 position)
{
    if (!SourceExists(entity_id))
    {
        Log::error("Source for entity: {} doesn't exist.", entity_id);
        return;
    }

    ALuint source;
    source = diegetic_sources_[entity_id].first;

    alSource3f(source, AL_VELOCITY, 0, 0, 0);
    alSource3f(source, AL_POSITION,  //
               position.x, position.y, position.z);

    if (alGetError() != AL_NO_ERROR)
    {
        Log::error("Couldn't set source position for {}.", entity_id);
        return;
    }
}

void AudioService::SetListenerPosition(glm::vec3 position)
{
    alListener3f(AL_POSITION, position.x, position.y, position.z);

    if (alGetError() != AL_NO_ERROR)
    {
        Log::error("Couldn't set listener position.");
    }
}

void AudioService::SetListenerOrientation(glm::vec3 forward, glm::vec3 up)
{
    ALfloat orientation[] = {forward.x, forward.y, forward.z,  //
                             up.x,      up.y,      up.z};      //
    alListenerfv(AL_ORIENTATION, orientation);

    if (alGetError() != AL_NO_ERROR)
    {
        Log::error("Couldn't set listener orientation.");
    }
}

/* ----- backend ----- */

AudioFile AudioService::LoadAudioFile(std::string file_name, bool is_music)
{
    // determine which folder to search from
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
    ALshort* file_data;

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

    // determine format
    // (unless something is terribly wrong, should always be 16 bits)
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

void AudioService::CullSources()
{
    // iterate through active sources
    for (auto pair = non_diegetic_sources_.begin(),
              next_pair = pair;                //
         pair != non_diegetic_sources_.end();  //
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

            non_diegetic_sources_.erase(pair);
        }
    }
}

void AudioService::UpdateStreamBuffer()
{
    ALuint source = music_source_.second.first;
    ALint buffers_processed;
    alGetSourcei(source, AL_BUFFERS_PROCESSED, &buffers_processed);

    // we haven't streamed a buffer yet
    if (buffers_processed <= 0)
    {
        return;
    }

    auto audio_data = music_file_.data_.get();

    // for every buffer in queue that was played
    while (buffers_processed > 0)
    {
        // pop off already played buffer
        ALuint buffer;
        alSourceUnqueueBuffers(source, 1, &buffer);

        // reserve memory for new audio data
        ALshort* new_data = new ALshort[kStreamBufferSize / sizeof(short)];
        std::memset(new_data, 0, kStreamBufferSize);

        ALsizei new_data_size = kStreamBufferSize;

        // for when the remainder of the file is less than a buffer size
        if (playhead_ + kStreamBufferSize > music_file_.GetSizeBytes())
        {
            new_data_size = music_file_.GetSizeBytes() - playhead_;
        }

        // get new data
        std::memcpy(&new_data[0], &audio_data[playhead_ / sizeof(short)],
                    new_data_size);

        // advance playhead
        playhead_ += new_data_size;

        // when remainder is less than buffer size, fill buffer with the
        // beginning of the file for a seamless loop
        if (new_data_size < kStreamBufferSize)
        {
            // loop back to beginning of song
            playhead_ = 0;
            int buffer_size = kStreamBufferSize - new_data_size;
            std::memcpy(&new_data[new_data_size],
                        &audio_data[playhead_ / sizeof(short)], buffer_size);
        }

        // copy new data into buffer and queue it
        alBufferData(buffer, music_file_.format_, new_data, kStreamBufferSize,
                     music_file_.sample_rate_);
        alSourceQueueBuffers(source, 1, &buffer);
        if (alGetError() != AL_NO_ERROR)
            Log::error("Couldn't stream audio.");

        // clean up
        delete[] new_data;
        buffers_processed--;
    }
}

bool AudioService::IsPlaying(std::string file_name)
{
    ALuint source = non_diegetic_sources_[file_name].first;
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

bool AudioService::IsPlaying(std::uint32_t entity_id)
{
    ALuint source = diegetic_sources_[entity_id].first;
    ALint source_state;
    alGetSourcei(source, AL_SOURCE_STATE, &source_state);

    if (alGetError() != AL_NO_ERROR)
    {
        Log::warning("Couldn't get Source State from entity: {}.", entity_id);
    }

    if (source_state != AL_PLAYING)
    {
        return false;
    }

    return true;
}

bool AudioService::SourceExists(std::string file_name)
{
    if (non_diegetic_sources_.count(file_name) == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool AudioService::SourceExists(std::uint32_t entity_id)
{
    if (diegetic_sources_.count(entity_id) == 1)
    {
        return true;
    }
    else
    {
        return false;
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

    playhead_ = 0;

    // set distance model, apparently this one is the best?
    alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
}

void AudioService::OnStart(ServiceProvider& service_provider)
{
    // debugging
    input_service_ = &service_provider.GetService<InputService>();
}

void AudioService::OnSceneLoaded(Scene& scene)
{
    // debugging
    SetMusic(kTestMusic);
    PlayMusic(kTestMusic);
}

void AudioService::OnUpdate()
{
    // check if there's something to update
    if (music_source_.second.first != NULL)
    {
        UpdateStreamBuffer();
    }

    // clear sources not playing
    CullSources();
}

void AudioService::OnCleanup()
{
    // clear music
    alDeleteSources(1, &music_source_.second.first);
    alDeleteBuffers(kStreamBufferAmount, music_source_.second.second);
    delete[] music_source_.second.second;

    // clear sfx
    for (auto& source : non_diegetic_sources_)
    {
        alDeleteSources(1, &source.second.first);
        alDeleteBuffers(1, &source.second.second);
    }

    for (auto& source : diegetic_sources_)
    {
        alDeleteSources(1, &source.second.first);
        alDeleteBuffers(1, &source.second.second);
    }

    // clear context + device
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(audio_context_);
    alcCloseDevice(audio_device_);
}

std::string_view AudioService::GetName() const
{
    return "AudioService";
}
