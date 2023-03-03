#pragma once

#include <AL/alc.h>

#include "engine/service/Service.h"

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
    ALCdevice* alc_device_;
};