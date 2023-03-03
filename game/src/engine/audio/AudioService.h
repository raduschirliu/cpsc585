#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#include <AudioFile.h>

#include <object_ptr.hpp>
#include <string>

#include "engine/input/InputService.h"
#include "engine/service/Service.h"
#include "engine/service/ServiceProvider.h"

class AudioService final : public Service
{
  public:
    AudioService();

    // from service:
    void OnInit() override;
    void OnStart(ServiceProvider& service_provider) override;
    void OnSceneLoaded(Scene& scene) override;
    void OnUpdate() override;
    void OnCleanup() override;
    std::string_view GetName() const override;

    /// @brief plays a soundfile fully just once.
    /// @param file_path
    /// @param gain optionally set the gain compensation. default: 0
    void PlayOneShot(std::string file_path, float gain = 0.f);

    /// @brief loops a soundfile indefinitely (until stopped by other function).
    /// @param file_path
    /// @param gain optionally set the gain compensation. default: 0
    void PlayLoop(std::string file_path, float gain = 0.f);

    /// @brief stops all currently playing sounds
    void StopAll();

  private:
    jss::object_ptr<InputService> input_service_;

    /// @brief the sound device to output game audio to.
    ALCdevice* audio_device_;
    /// @brief it's like an openGL context.
    ALCcontext* audio_context_;
    ALuint buffer_;

    enum class AudioType
    {
        ONESHOT,
        LOOP
    };

    /// @brief loads file from the directory corresponding to the audio_type
    /// @param audio_type either a ONESHOT or a LOOP
    AudioFile<float> LoadAudioFile(std::string file_name, AudioType audio_type);
    /// @brief gets the format of the file (mono/stereo, 8/16 bit)
    ALenum GetAudioFileFormat(AudioFile audio_file);
    /// @brief gets the audio file's raw data
    float GetAudioFileData(AudioFile audio_file);
};