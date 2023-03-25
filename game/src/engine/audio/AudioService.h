#pragma once

#include <AL/alc.h>

#include <glm/glm.hpp>
#include <map>

#include "AudioFile.h"
#include "engine/service/Service.h"

class AudioService final : public Service
{
  public:
    /* ----- sfx playback ----- */

    /**
     *  add a source to play an audio file from.
     *
     *  @param file_name name of the audio file to set a source to.
     */
    void AddSource(std::string file_name);

    /**
     *  add a 3D source to play an audio file from, identified by an entity's
     * id.
     *
     *  @param entity_id the id of the entity to track its position from.
     *
     *  @overload
     */
    void AddSource(uint32_t entity_id, std::string file_name);

    /**
     *  play an audio file through its source.
     *
     *  @param file_name name of the audio file to play from.
     */
    void PlaySource(std::string file_name);

    /**
     *  play an audio file through its source, identified by
     *  its associated entity id.
     *
     *  @param entity_id the id of the associated entity.
     */
    void PlaySource(uint32_t entity_id, std::string file_name);

    /**
     *  stop a sources playback of an audio file.
     *
     *  @param file_name name of the audio file to stop playback of.
     */
    void StopSource(std::string file_name);

    /**
     *  stop a sources playback of an audio file, identified by
     *  its associated entity id.
     *
     *  @overload
     */
    void StopSource(uint32_t entity_id, std::string);

    /// stop the playback of all sources (excluding the music source).
    void StopAllSources();

    /* ----- music playback ----- */

    /**
     *  add a source to stream music from.
     *
     *  @param file_name name of the music file to set the source to.
     */
    void SetMusic(std::string file_name);

    /**
     *  begin streaming a music file through a source.
     *
     *  @note must set the music first before calling.
     *  @note can only have one file set at a time.
     *
     *  @see SetMusic();
     */

    void PlayMusic();

    /// stop the playback of music.
    void StopMusic();

    /* ----- setting source properties ----- */

    /**
     *  sets the overall game volume.
     *
     *  @param gain relative gain compensation to be applied.
     *
     *  @note gain is given as a positive float relative to 1 (i.e. a value of 1
     *    has no effect, > 1 is louder, and so on).
     */
    void SetMasterGain(float gain);

    /**
     *  sets whether or not the playback of an audio file loops.
     *
     *  @param file_name name of the audio file to play from (extension
     *    optional).
     *  @param is_looping whether or not the playback loops.
     */
    void SetLoop(std::string file_name, bool is_looping);

    /**
     *  sets whether or not the playback of an audio file loops.
     *
     *  @overload
     */
    void SetLoop(uint32_t entity_id, std::string file_name, bool is_looping);

    /**
     *  sets the gain of an audio file's playback.
     *
     *  @param file_name name of the audio file.
     *  @param gain relative gain compensation to be applied.
     *
     *  @note gain is given as a positive float relative to 1 (i.e. a value of 1
     *    has no effect, > 1 is louder, and so on).
     */
    void SetGain(std::string file_name, float gain);

    /**
     *  sets the gain of an audio file's playback.
     *
     *  @overload
     */
    void SetGain(uint32_t entity_id, std::string file_name, float gain);

    /**
     *  sets the gain of the music's playback.
     *
     *  @param gain relative gain compensation to be applied.
     *
     *  @note gain is given as a positive float relative to 1 (i.e. a value of 1
     *    has no effect, > 1 is louder, and so on).
     */
    void SetMusicGain(float gain);

    /**
     *  offsets the pitch of an audio file.
     *
     *  @param file_name name of the audio file.
     *  @param pitch_offset relative pitch offset to be applied.
     *
     *  @note pitch_offset is given as a positive float relative to 1
     *    (i.e. a value of 1 has no effect, > 1 is higher pitch, and so on).
     */
    void SetPitch(std::string file_name, float pitch_offset);

    /**
     *  offsets the pitch of an audio file.
     *
     *  @overload
     */
    void SetPitch(uint32_t entity_id, std::string file_name,
                  float pitch_offset);

    /**
     *  set the world position of a source to play from.
     *
     *  @param entity_id the sources associated entity id.
     *  @param position the position to set the source at.
     */
    void SetSourcePosition(uint32_t entity_id, glm::vec3 position);

    /* ----- listener properties ------ */

    /**
     *  set the position of the listener.
     *
     *  @param position the position to set the listener at.
     */
    void SetListenerPosition(glm::vec3 position);

    /**
     *  set the orientation of the listener.
     *
     *  @param forward the forward vector of the listener.
     *  @param up the up vector of the listener.
     *
     *  @see SetListenerPosition
     */
    void SetListenerOrientation(glm::vec3 forward, glm::vec3 up);

    /* ----- from service ----- */

    void OnInit() override;
    void OnStart(ServiceProvider& service_provider) override;
    void OnSceneLoaded(Scene& scene) override;
    void OnUpdate() override;
    void OnCleanup() override;
    std::string_view GetName() const override;

  private:
    /**
     *  load an audiofile's data into memory from the appropriate directory.
     *
     *  @param file_name name of the audio file to load.
     *  @param is_music whether the specified file is music or not.
     *  @return an AudioFile struct that contains all the extrapolated
     *    information on the file.
     *
     *  @note the distinction between music and not is important because
     *    AudioService streams from music files instead of playing them directly
     *    like SFX.
     */
    AudioFile LoadAudioFile(std::string file_name, bool is_music);

    /**
     *  if currently streaming music checks if any stream buffers have been used
     *  and updates them accordingly.
     *
     *  @note ideally called every timestep.
     */
    void UpdateStreamBuffer();

    /// deletes all inactive sources and buffers
    void CullSources();
    /// OpenAl error handling
    bool CheckAlError(std::string error_message = "");

    /* ----- getters ----- */

    /// @brief determines whether or not an audio file's source
    ///   is currently playing.
    /// @param file_name name of the audio file the source may be playing.
    bool IsPlaying(std::string file_name);

    /// @brief determines whether or not an audio file's source
    ///   is currently playing.
    /// @overload
    bool IsPlaying(uint32_t entity_id, std::string file_name);

    /// @brief determines whether or not a source with a specified audio file
    ///   already exists.
    /// @param file_name name of the audio file to check for.
    bool SourceExists(std::string file_name);

    /// @brief determines whether or not a source with an associated entity id
    ///   already exists.
    /// @overload
    bool SourceExists(uint32_t entity_id, std::string file_name);

    /* ------ members ------ */

    ALCdevice* audio_device_;    // the sound device to output audio to.
    ALCcontext* audio_context_;  // like an openGL context.

    /// all of the active 2D sound sources.
    using SourceBufferPair = std::pair<ALuint, ALuint>;
    using FileName = std::string;
    using EntityID = uint32_t;
    std::map<FileName, SourceBufferPair> non_diegetic_sources_;

    /// all of the active 3D/spatial sound sources and their entity's id.
    using NameSourceMap = std::map<FileName, SourceBufferPair>;
    std::map<EntityID, NameSourceMap> diegetic_sources_;

    /// the current music file to stream from
    AudioFile music_file_;
    /// the current music source.
    std::pair<std::string, std::pair<ALuint, ALuint*>> music_source_;
    /// keep track of how much of the file was played
    ALsizei playhead_;
};
