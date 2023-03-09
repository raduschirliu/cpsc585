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

/**
 *  @todo streaming for longer audio files
 *  @todo implement positional audio
 */
class AudioService final : public Service
{
  public:
    /**
     *  plays an audio file fully just once.
     *
     *  @note only accepts mono files.
     *
     *  @param file_name name of audio file (including extension).
     *  @param gain relative gain compensation to be added. default: 1.f
     */
    void PlayOneShot(std::string file_name, float gain = 1.f);

    /**
     *  @todo
     *  plays and loops an audio file until explicitly stopped.
     *
     *  @note only accepts mono files.
     *
     *  @param file_name name of audio file (including extension).
     *  @param gain relative gain compensation to be added. default: 1.f
     */
    void PlayLoop(std::string file_name, float gain = 1.f);

    /**
     *  @todo
     *  plays and loops music, with no 3D positional effect.
     *
     *  @param file_name name of audio file (including extension).
     *  @param gain relative gain compensation to be added. default: 1.f
     */
    void PlayMusic(std::string file_name, float gain = 1.f);

    /**
     *  stops an audio file's playback.
     *
     *  @param file_name name of audio file (including extension).
     */
    void StopPlayback(std::string file_name);

    /// @brief stops playback of all sound files.
    void StopAllPlayback();

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

    /// @brief creates and adds a source and buffer for the file given.
    void AddSource(std::string file_name, bool is_looping = false);
    
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
    /// @note <file_name, <source, buffer>>
    std::map<std::string, std::pair<ALuint, ALuint>> active_sources_;

    /// @brief loads file from the directory corresponding to the audio_type.
    AudioFile LoadAudioFile(std::string file_name, bool is_music = false);

    bool IsPlaying(std::string file_name);

    /// @brief deletes inactive sources and buffers
    void CullSources();
};
