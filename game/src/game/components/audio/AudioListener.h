#pragma once

#include "engine/audio/AudioService.h"
#include "engine/fwd/FwdComponents.h"
#include "engine/fwd/FwdServices.h"
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"

class AudioListener final : public Component
{
  public:
    /* ----- from Component -----*/

    void OnInit(const ServiceProvider& service_provider) override;
    std::string_view GetName() const override;

  private:
    jss::object_ptr<AudioService> audio_service_;
};