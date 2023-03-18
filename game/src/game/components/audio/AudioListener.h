#pragma once

#include "engine/FwdComponents.h"
#include "engine/FwdServices.h"
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"

class AudioListener final : public Component,
                            public IEventSubscriber<OnUpdateEvent>
{
  public:
    /* ----- from Component -----*/

    void OnInit(const ServiceProvider& service_provider) override;
    std::string_view GetName() const override;

    /* ----- from IEventSubscriber ----- */

    void OnUpdate(const Timestep& delta_time) override;

  private:
    jss::object_ptr<AudioService> audio_service_;
    jss::object_ptr<Transform> transform_;
};