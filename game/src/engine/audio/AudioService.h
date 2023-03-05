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
    AudioService();

    /// @brief plays a soundfile fully just once.
    /// @param gain optionally set the gain compensation. default: 0
    void PlayOneShot(std::string file_name, int gain = 0);

    /// @brief loops a soundfile indefinitely (until stopped).
    /// @param gain optionally set the gain compensation. default: 0
    void PlayLoop(std::string file_name, int gain = 0);

    /// @todo
    /// @brief stops playback of a specified sound. 
    void StopPlayback(std::string file_name);

    /// @todo
    /// @brief stops playback of all sounds.
    void StopAllPlayback();

    // from service:
    void OnInit() override;
    void OnStart(ServiceProvider& service_provider) override;
    void OnSceneLoaded(Scene& scene) override;
    void OnUpdate() override;
    void OnCleanup() override;
    std::string_view GetName() const override;

  private:
    jss::object_ptr<InputService> input_service_;

    /// @brief the sound device to output game audio to.
    ALCdevice* audio_device_;
    /// @brief it's like an openGL context.
    ALCcontext* audio_context_;
    /// @brief all of the currently active sources.
    /// @note <file_name, <source, buffer>>
    std::map<std::string, std::pair<ALuint, ALuint>> sources_;

    /// @brief loads file from the directory corresponding to the audio_type.
    /// @param audio_type either a ONESHOT or a LOOP.
    AudioFile LoadAudioFile(std::string file_name, bool is_looping);
    /// @brief gets the format of the file (mono/stereo, 8/16 bit).
    // ALenum GetFormat(AudioData* data);
    /// @brief gets the audio file's raw data.
    // std::vector<float> GetData(AudioData* data);
};
