#pragma once

#include <AL/al.h>
#include <AL/alc.h>

#include <map>
#include <object_ptr.hpp>
#include <string>
#include <utility>

#include "AudioFile.h"
#include "engine/input/InputService.h"
#include "engine/service/Service.h"
#include "engine/service/ServiceProvider.h"

class AudioService final : public Service
{
  public:
    /* ----- set sources functions -----*/

    /**
     *  add a source to play an audio file from.
     *
     *  @param file_name name of audio file (including extension).
     */
    void AddSource(std::string file_name);

    /**
     *  add a source to stream music from
     *
     *  @param file_name name of music file (including extension).
     *
     *  @note AudioService streams from just one music file at a time.
     */
    void SetMusic(std::string file_name);

    /* ----- playback functions ----- */

    /**
     *  plays an audio file through a source
     *
     *  @param file_name name of audio file (including extension).
     */
    void PlaySource(std::string file_name);

    /**
     *  streams a music file through a source
     *
     *  @param file_name name of music file (including extension).
     */
    void PlayMusic(std::string file_name);

    /// @brief stops a sources playback.
    void StopSource(std::string file_name);

    /// @brief stops playback of all sources.
    void StopAllSources();

    void StopMusic();

    /* ----- public setters ----- */

    /// @brief whether given source loops or not
    void SetLooping(std::string file_name, bool is_looping);

    /**
     *  sets the gain of a source specified by its file name.
     *
     *  @param file_name name of audio file (including extension).
     *  @param gain relative gain compensation to be added.
     */
    void SetGain(std::string file_name, float gain);

    /**
     *  offsets the pitch of a source specified by its file name.
     *
     *  @param file_name name of audio file (including extension).
     *  @param pitch_offset the amount to change pitch.
     */
    void SetPitch(std::string file_name, float pitch_offset);

    /* ----- from service ----- */

    void OnInit() override;
    void OnStart(ServiceProvider& service_provider) override;
    void OnSceneLoaded(Scene& scene) override;
    void OnUpdate() override;
    void OnCleanup() override;
    std::string_view GetName() const override;

  private:
    /// @note post-debugging we prob want to remove this
    jss::object_ptr<InputService> input_service_;

    /// @brief the sound device to output game audio to.
    ALCdevice* audio_device_;

    /// @brief it's like an openGL context.
    ALCcontext* audio_context_;

    /// @brief all of the currently active sources.
    std::map<std::string, std::pair<ALuint, ALuint>> active_sources_;

    /// @brief the current music source.
    std::pair<std::string, std::pair<ALuint, ALuint*>> music_source_;

    /// @brief loads file from the appropriate directory.
    AudioFile LoadAudioFile(std::string file_name, bool is_music = false);

    /**
     *  when streaming music, pops off used buffers in queue and fills them with
     *  new audio data.
     *
     *  @note ideally should be called every timestep
     */
    void UpdateStreamBuffer();

    /// @brief deletes inactive sources and buffers
    void CullSources();

    /* ----- helpers ----- */

    bool IsPlaying(std::string file_name);
    bool SourceExists(std::string file_name);
};
