#pragma once

#include <AL/al.h>
#include <AL/alc.h>

#include <glm/glm.hpp>
#include <map>
#include <object_ptr.hpp>
#include <optional>
#include <string>
#include <utility>

#include "AudioFile.h"
#include "engine/input/InputService.h"
#include "engine/scene/Entity.h"
#include "engine/scene/Transform.h"
#include "engine/service/Service.h"
#include "engine/service/ServiceProvider.h"

class AudioService final : public Service
{
    typedef ALuint Source;
    typedef ALuint Buffer;

  public:
    /* ----- sources + listener functions -----*/

    /**
     *  add a source to play an audio file from.
     *
     *  @param file_name name of audio file (including extension).
     */
    void AddSource(std::string file_name);

    /**
     *  add a source to stream music from.
     *
     *  @param file_name name of music file (including extension).
     *
     *  @note AudioService streams from just one music file at a time.
     */
    void SetMusic(std::string file_name);

    void SetListener(Entity& entity);

    /* ----- playback functions ----- */

    /**
     *  plays an audio file through a source.
     *
     *  @param file_name name of audio file (including extension).
     */
    void PlaySource(std::string file_name);

    /**
     *  streams a music file through a source.
     *
     *  @param file_name name of music file (including extension).
     *
     *  @note can only play one music file at a time.
     */
    void PlayMusic(std::string file_name);

    void StopSource(std::string file_name);
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

    void SetListenerPosition(glm::vec3 position);
    void SetListenerOrientation(glm::vec3 forward, glm::vec3 up);

    /* ----- from service ----- */

    void OnInit() override;
    void OnStart(ServiceProvider& service_provider) override;
    void OnSceneLoaded(Scene& scene) override;
    void OnUpdate() override;
    void OnCleanup() override;
    std::string_view GetName() const override;

  private:
    /// @note post-debugging we prob want to remove this.
    jss::object_ptr<InputService> input_service_;

    /// @brief the sound device to output game audio to.
    ALCdevice* audio_device_;

    /// @brief it's like an openGL context.
    ALCcontext* audio_context_;

    /// @brief entity of our listener (to get position for spatial audio).
    Entity& listener_;

    /// @brief all of the currently active sources.
    std::map<std::string, std::pair<Source, Buffer>> active_sources_;

    /// @brief the current music source.
    std::pair<std::string, std::pair<Source, ALuint*>> music_source_;

    /// @brief loads file from the appropriate directory.
    AudioFile LoadAudioFile(std::string file_name, bool is_music = false);

    /**
     *  when streaming music, pops off used buffers in queue and fills them with
     *  new audio data.
     *
     *  @note should be called every timestep
     */
    void UpdateStreamBuffer();

    /**
     *  update positions for positional audio.
     *
     *  @note should be called every timestep.
     */
    void UpdatePositions();

    /// @brief deletes inactive sources and buffers.
    void CullSources();

    /* ----- helpers ----- */

    bool IsPlaying(std::string file_name);
    bool SourceExists(std::string file_name);
};
