#pragma once

#include <AL/alc.h>

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
    /// @brief the sound device to output game audio to.
    ALCdevice* audio_device_;
    /// @brief it's like an openGL context.
    ALCcontext* audio_context_;
    jss::object_ptr<InputService> input_service_;
};