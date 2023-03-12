#pragma once

#include <object_ptr.hpp>
#include <string>

#include "engine/audio/AudioService.h"
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/Transform.h"

class SoundEmitter final : public Component,
                           public IEventSubscriber<OnUpdateEvent>
{
  public:
    void AddSource(std::string file_name);

    /* ----- from Component -----*/

    void OnInit(const ServiceProvider& service_provider) override;
    std::string_view GetName() const override;

    /* ----- from IEventSubscriber ----- */

    void OnUpdate(const Timestep& delta_time) override;

  private:
    jss::object_ptr<AudioService> audio_service_;
    jss::object_ptr<Transform> transform_;
};