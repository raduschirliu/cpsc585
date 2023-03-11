#pragma once

#include "engine/audio/AudioService.h"
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/Transform.h"

class Listener : public Component, public IEventSubscriber<OnUpdateEvent>
{
  public:
    /* ----- from Component -----*/

    virtual void OnInit(const ServiceProvider& service_provider) override;
    virtual void OnUpdate(const Timestep& delta_time) override;
    virtual void OnDestroy() override;

  private:
    jss::object_ptr<AudioService> audio_service_;
    jss::object_ptr<Transform> transform_;
};