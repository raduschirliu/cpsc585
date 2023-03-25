#include "AudioService.h"

#include "engine/core/debug/Log.h"
#include "engine/service/ServiceProvider.h"
#include "stb/stb_vorbis.h"

// the system's default output device will be used
static const ALCchar* kDefaultDevice = nullptr;
static ALCint kContextAttributes = 0;

// audio directories
static const std::string kSfxDirectory = "resources/audio/sfx/";
static const std::string kMusicDirectory = "resources/audio/music/";

// number of buffers to use when streaming music files
static constexpr std::size_t kStreamBufferAmount = 4;
static constexpr ALsizei kStreamBufferSize = 65536;  // 32kb per buffer

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

    if (CheckAlError())
    {
        debug::LogError("Couldn't buffer audio data for {}.", file_name);
        return;
    }

    // create and initialise source
    ALuint source;
    alGenSources(1, &source);
    alSourcei(source, AL_LOOPING, AL_FALSE);
    alSourcei(source, AL_BUFFER, buffer);  // GIVE SOURCE ITS BUFFER

    SourceBufferPair source_buffer_pair = {source, buffer};

    if (CheckAlError())
    {
        debug::LogError("Couldn't create audio source for {}", file_name);
        debug::LogError("Couldn't create audio source for {}", file_name);
        return;
    }

    non_diegetic_sources_.insert({file_name, source_buffer_pair});
}

void AudioService::AddSource(uint32_t entity_id, std::string file_name)
{
    AudioFile audio_file = LoadAudioFile(file_name, false);

    // create buffer in memory
    ALuint buffer;
    alGenBuffers(1, &buffer);

    // initialise buffer for audio data
    alBufferData(buffer, audio_file.format_, audio_file.data_.get(),
                 audio_file.GetSizeBytes(), audio_file.sample_rate_);

    if (CheckAlError())
    {
        debug::LogError("Couldn't buffer audio data for {}.", file_name);
        return;
        return;
    }

    // create and initialise source
    ALuint source;
    alGenSources(1, &source);
    alSourcei(source, AL_LOOPING, AL_FALSE);
    alSourcei(source, AL_BUFFER, buffer);  // GIVE SOURCE ITS BUFFER

    // set properties for spatial audio
    alSourcef(source, AL_MAX_DISTANCE, 400.0f);  // distance until silent
    alSourcef(source, AL_REFERENCE_DISTANCE, 100.0f);
    alSourcef(source, AL_ROLLOFF_FACTOR, 0.5f);

    SourceBufferPair source_buffer_pair = {source, buffer};

    if (CheckAlError())
    {
        debug::LogError("Couldn't create source for {}", file_name);
        return;
        return;
    }

    diegetic_sources_[entity_id][file_name] = source_buffer_pair;
}

void AudioService::SetMusic(std::string file_name)
{
    music_file_ = LoadAudioFile(file_name, true);

    // reserve buffers in memory
    ALuint* buffers = new ALuint[kStreamBufferAmount];
    alGenBuffers(kStreamBufferAmount, &buffers[0]);

    if (CheckAlError())
    {
        debug::LogError("Couldn't create buffer stream for {}.", file_name);
        debug::LogError("Couldn't create buffer stream for {}.", file_name);
        return;
    }

    // initialise buffers for audio data
    auto audio_data = music_file_.data_.get();

    for (std::size_t i = 0; i < kStreamBufferAmount; i++)
    {
        alBufferData(buffers[i], music_file_.format_,
                     &audio_data[(i * kStreamBufferSize) / sizeof(short)],
                     kStreamBufferSize, music_file_.sample_rate_);

        if (CheckAlError())
        {
            debug::LogError("Couldn't buffer audio data for {}.", file_name);
            return;
            return;
        }
    }
    playhead_ = kStreamBufferSize * kStreamBufferAmount;

    // create and initialise source
    ALuint source;
    alGenSources(1, &source);
    // don't want to loop just one buffer
    alSourcei(source, AL_LOOPING, AL_FALSE);

    if (CheckAlError())
    {
        debug::LogError("Couldn'create audio source for {}.", file_name);
        debug::LogError("Couldn'create audio source for {}.", file_name);
        return;
    }

    // queue buffers for source
    alSourceQueueBuffers(source, kStreamBufferAmount, &buffers[0]);

    if (CheckAlError())
    {
        debug::LogError("Couldn't queue audio buffers for {}.", file_name);
        return;
        return;
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

    if (CheckAlError())
    {
        debug::LogError("Couldn't play audio for {}.", file_name);
        return;
    }

    debug::LogDebug("Playing audio: {}", file_name);
}

void AudioService::PlaySource(uint32_t entity_id, std::string file_name)
{
    // check if source already exists;
    if (!SourceExists(entity_id, file_name))
    {
        debug::LogWarn("Source for Entity {} and file {} doesn't exist yet.",
                       entity_id, file_name);
        return;
    }

    // find the source
    ALuint source;
    source = diegetic_sources_[entity_id][file_name].first;

    alSourcePlay(source);

    if (CheckAlError())
    {
        debug::LogError("Entity {} couldn't play audio for {}.", entity_id,
                        file_name);
        return;
    }

    debug::LogDebug("Entity {} playing audio.", entity_id);

    float x, y, z;

    alGetSource3f(source, AL_POSITION, &x, &y, &z);
    debug::LogDebug("Source position: {}, {}, {}", x, y, z);

    alGetListener3f(AL_POSITION, &x, &y, &z);
    debug::LogDebug("AudioListener position: {}, {}, {}", x, y, z);
}

void AudioService::PlayMusic()
{
    //  music wasn't set yet
    if (music_source_.first == "")
    {
        debug::LogWarn("Source wasn't set yet");
        return;
    }

    // find the source
    ALuint source;
    source = music_source_.second.first;

    alSourcef(source, AL_GAIN, 0.2f);
    alSourcePlay(source);

    if (CheckAlError())
    {
        debug::LogError("Couldn't play music.");
        return;
    }

    debug::LogDebug("Starting playing music.");
}

void AudioService::StopSource(std::string file_name)
{
    ALuint source = non_diegetic_sources_[file_name].first;
    alSourceStop(source);
}

void AudioService::StopSource(uint32_t entity_id, std::string file_name)
{
    ALuint source = diegetic_sources_[entity_id][file_name].first;
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
    std::string file_name = music_source_.first;
    if (!SourceExists(file_name))
    {
        debug::LogError("Music wasn't set yet.");
        return;
    }

    ALuint source = music_source_.second.first;
    alSourceStop(source);
}

/* ----- setters ------ */

void AudioService::SetMasterGain(float gain)
{
    alListenerf(AL_GAIN, gain);

    if (CheckAlError())
    {
        debug::LogError("Couldn't set master volume.");
    }
}

void AudioService::SetPitch(std::string file_name, float pitch_offset)
{
    if (!SourceExists(file_name))
    {
        debug::LogError("Source wasn't set for {}", file_name);
        debug::LogError("Source wasn't set for {}", file_name);
        return;
    }

    ALuint source;
    source = non_diegetic_sources_[file_name].first;

    alSourcef(source, AL_PITCH, pitch_offset);

    if (CheckAlError())
    {
        debug::LogError("Couldn't set pitch for {}.", file_name);
    }

    debug::LogDebug("Offset pitch of {} by {}", file_name, pitch_offset);
}

void AudioService::SetPitch(uint32_t entity_id, std::string file_name,
                            float pitch_offset)
{
    if (!SourceExists(entity_id, file_name))
    {
        debug::LogError("Source wasn't set for Entity {} and file {}",
                        entity_id, file_name);
        return;
    }

    ALuint source;
    source = diegetic_sources_[entity_id][file_name].first;

    alSourcef(source, AL_PITCH, pitch_offset);

    if (CheckAlError())
    {
        debug::LogError("Couldn't set pitch for {}.", entity_id);
    }

    debug::LogDebug("Offset pitch for Entity: {}'s sound {} by {}", entity_id,
                    file_name, pitch_offset);
}

void AudioService::SetGain(std::string file_name, float gain)
{
    if (!SourceExists(file_name))
    {
        debug::LogError("Source wasn't set for {}", file_name);
        return;
    }

    ALuint source;
    source = non_diegetic_sources_[file_name].first;

    alSourcef(source, AL_GAIN, gain);

    if (CheckAlError())
    {
        debug::LogError("Couldn't set gain for {}.", file_name);
    }

    debug::LogDebug("Set gain of {} set {}", file_name, gain);
}

void AudioService::SetGain(uint32_t entity_id, std::string file_name,
                           float gain)
{
    if (!SourceExists(entity_id, file_name))
    {
        debug::LogError("Source wasn't set for Entity {} and file {}",
                        entity_id, file_name);
        return;
    }

    ALuint source;
    source = diegetic_sources_[entity_id][file_name].first;

    alSourcef(source, AL_GAIN, gain);

    if (CheckAlError())
    {
        debug::LogError("Couldn't set gain for {}.", entity_id);
    }

    debug::LogDebug("Set gain for Entity: {}'s sound {} by {}", entity_id,
                    file_name, gain);
}

void AudioService::SetMusicGain(float gain)
{
    std::string file_name = music_source_.first;
    if (!SourceExists(file_name))
    {
        debug::LogError("Music wasn't set yet.");
        return;
    }

    ALuint source = music_source_.second.first;
    alSourcef(source, AL_GAIN, gain);

    if (CheckAlError())
    {
        debug::LogError("Couldn't set gain for {}.", file_name);
    }

    debug::LogDebug("Set gain of {} set {}", file_name, gain);
}

void AudioService::SetLoop(std::string file_name, bool is_looping)
{
    if (!SourceExists(file_name))
    {
        debug::LogError("Source wasn't set for {}", file_name);
        return;
    }

    ALuint source;
    source = non_diegetic_sources_[file_name].first;

    if (is_looping)
    {
        alSourcef(source, AL_LOOPING, AL_TRUE);
        debug::LogDebug("Set {} to loop.", file_name);
    }
    else
    {
        alSourcef(source, AL_LOOPING, AL_FALSE);
        debug::LogDebug("Set {} to not loop.", file_name);
    }
}

void AudioService::SetLoop(uint32_t entity_id, std::string file_name,
                           bool is_looping)
{
    if (!SourceExists(entity_id, file_name))
    {
        debug::LogError("Source wasn't set for Entity {} and file {}",
                        entity_id, file_name);
        return;
    }

    ALuint source;
    source = diegetic_sources_[entity_id][file_name].first;

    if (is_looping)
    {
        alSourcef(source, AL_LOOPING, AL_TRUE);
        debug::LogDebug("Set source for Entity {}'s sound {} to loop.",
                        entity_id, file_name);
    }
    else
    {
        alSourcef(source, AL_LOOPING, AL_FALSE);
        debug::LogDebug("Set source for Entity {}'s sound {} to not loop.",
                        entity_id, file_name);
    }
}

void AudioService::SetSourcePosition(uint32_t entity_id, glm::vec3 position)
{
    for (auto& source : diegetic_sources_[entity_id])
    {
        std::string file_name = source.first;
        if (!SourceExists(entity_id, file_name))
        {
            debug::LogError("Source for Entity {} and file {} doesn't exist.",
                            entity_id, file_name);
            return;
        }

        ALuint source;
        source = diegetic_sources_[entity_id][file_name].first;

        alSource3f(source, AL_VELOCITY, 0, 0, 0);
        alSource3f(source, AL_POSITION,  //
                   position.x, position.y, position.z);

        if (CheckAlError())
        {
            debug::LogError(
                "Couldn't set source position for Entity {} and file {}",
                entity_id, file_name);
            return;
        }
    }
}

void AudioService::SetListenerPosition(glm::vec3 position)
{
    alListener3f(AL_POSITION, position.x, position.y, position.z);

    if (CheckAlError())
    {
        debug::LogError("Couldn't set listener position.");
    }
}

void AudioService::SetListenerOrientation(glm::vec3 forward, glm::vec3 up)
{
    ALfloat orientation[] = {forward.x, forward.y, forward.z,  //
                             up.x,      up.y,      up.z};      //
    alListenerfv(AL_ORIENTATION, orientation);

    if (CheckAlError())
    {
        debug::LogError("Couldn't set listener orientation.");
    }
}

/* ----- backend ----- */

AudioFile AudioService::LoadAudioFile(std::string file_name, bool is_music)
{
    // determine which folder to search from
    std::string file_path = (is_music) ? kMusicDirectory : kSfxDirectory;
    file_path.append(file_name);

    // initialize AudioFile
    AudioFile audio_file;
    ALshort* file_data;

    audio_file.samples_per_channel = stb_vorbis_decode_filename(
        file_path.c_str(), &audio_file.number_of_channels_,
        &audio_file.sample_rate_, &file_data);
    audio_file.data_ = std::unique_ptr<short>(file_data);

    debug::LogDebug("Succesfully opened audio file: {}", file_name);
    debug::LogDebug(
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
            debug::LogDebug("{} stopped playing.", pair->first);
            alDeleteSources(1, &pair->second.first);
            alDeleteBuffers(1, &pair->second.second);

            if (CheckAlError())
            {
                debug::LogError("While culling Sources for {}.", pair->first);
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

    if (CheckAlError())
    {
        debug::LogError("Couldn't get processed buffers for music streaming.");
        return;
    }

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
        if (CheckAlError())
            debug::LogError("Couldn't stream audio.");

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

    if (CheckAlError())
    {
        debug::LogWarn("Couldn't get Source State from {}.", file_name);
    }

    if (source_state != AL_PLAYING)
    {
        return false;
    }

    return true;
}

bool AudioService::IsPlaying(uint32_t entity_id, std::string file_name)
{
    ALuint source = diegetic_sources_[entity_id][file_name].first;
    ALint source_state;
    alGetSourcei(source, AL_SOURCE_STATE, &source_state);

    if (CheckAlError())
    {
        debug::LogWarn("Couldn't get Source State from entity: {}.", entity_id);
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

bool AudioService::SourceExists(uint32_t entity_id, std::string file_name)
{
    if (diegetic_sources_[entity_id].count(file_name))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool AudioService::CheckAlError(std::string error_message)
{
    ALenum al_error = alGetError();

    if (al_error == AL_NO_ERROR)
    {
        return false;
    }

    if (error_message != "")
    {
        debug::LogError(error_message.append(""));
    }

    switch (al_error)
    {
        case AL_INVALID_NAME:
            debug::LogError(
                "AL_INVALID_NAME: "
                "a bad name (ID) was passed to an OpenAL function.");
            break;
        case AL_INVALID_ENUM:
            debug::LogError(
                "AL_INVALID_ENUM: "
                "an invalid enum value was passed to an OpenAL function.");
            break;
        case AL_INVALID_VALUE:
            debug::LogError(
                "AL_INVALID_VALUE: "
                "an invalid value was passed to an OpenAL function.");
            break;
        case AL_INVALID_OPERATION:
            debug::LogError(
                "AL_INVALID_OPERATION: "
                "the requested operation is not valid.");
            break;
        case AL_OUT_OF_MEMORY:
            debug::LogError(
                "AL_OUT_OF_MEMORY: "
                "the requested operation resulted in"
                "OpenAL running out of memory.");
            break;
    }
    return true;
}

/* ----- from Service ------ */

void AudioService::OnInit()
{
    // open and verify audio device
    audio_device_ = alcOpenDevice(kDefaultDevice);

    ASSERT_MSG(audio_device_, "Couldn't open audio device.");
    debug::LogInfo("Successfuly opened audio device!");

    // create audio context
    audio_context_ = alcCreateContext(audio_device_, nullptr);
    alcMakeContextCurrent(audio_context_);

    if (CheckAlError())
    {
        // i.e no audio at all
        debug::LogError("Coudn't make audio context current.");
    }

    playhead_ = 0;

    // set distance model, apparently this one is the best?
    alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
}

void AudioService::OnStart(ServiceProvider& service_provider)
{
}

void AudioService::OnSceneLoaded(Scene& scene)
{
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

    for (auto& entity : diegetic_sources_)
    {
        for (auto& source : entity.second)
        {
            alDeleteSources(1, &source.second.first);
            alDeleteBuffers(1, &source.second.second);
        }
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
