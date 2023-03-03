#pragma once

#include <AL/alc.h>
#include <AL/al.h>
#include <AudioFile.h>

#include <object_ptr.hpp>

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

  private:
    jss::object_ptr<InputService> input_service_;

    /// @brief the sound device to output game audio to.
    ALCdevice* audio_device_;
    /// @brief it's like an openGL context.
    ALCcontext* audio_context_; 
    ALuint buffer_;
    AudioFile<float> audio_file_;

    /// @brief  plays a soundfile just once
    /// @param file_path 
    void PlayOneShot(std::string file_path);

    /// @brief gets the format of the file (mono/stereo, 8/16 bit)
    /// @param audio_file 
    ALenum GetAudioFileFormat(AudioFile audio_file);
    /// @brief gets the audio file's raw data 
    /// @param audio_file 
    double GetAudioFileData(AudioFile audio_file);
};