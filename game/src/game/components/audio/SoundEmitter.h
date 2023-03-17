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
    void SetSource(std::string file_name);
    void PlaySource();

    void SetPitch(float pitch_offset);
    void SetGain(float gain); 

    /* ----- from Component -----*/

    void OnInit(const ServiceProvider& service_provider) override;
    std::string_view GetName() const override;

    /* ----- from IEventSubscriber ----- */

    void OnUpdate(const Timestep& delta_time) override;

  private:
    std::string file_name_;
    jss::object_ptr<AudioService> audio_service_;
    jss::object_ptr<Transform> transform_;
};