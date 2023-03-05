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
    /// @brief plays a soundfile fully just once.
    /// @param gain optionally set the gain compensation. default: 0
    void PlayOneShot(std::string file_name, float gain = 1.f);

    /// @todo implement streaming audio
    /// instead buffering for longer files (i.e music, etc.)
    void PlayLoop(std::string file_name, float gain = 1.f);

    /// @todo
    /// stop playback of a specified sound.
    void StopPlayback(std::string file_name);

    /// @todo
    /// stop playback of all sounds.
    void StopAllPlayback();

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
    AudioFile LoadAudioFile(std::string file_name, bool is_looping = false);
    /// @brief creates and adds a source and buffer for the file given.
    void AddSource(std::string file_name, bool is_looping = false);
    bool IsPlaying(std::string file_name);
};
