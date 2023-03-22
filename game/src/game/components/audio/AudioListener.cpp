#include "AudioListener.h"

#include "engine/audio/AudioService.h"
#include "engine/core/debug/Log.h"
#include "engine/scene/Component.h"
#include "engine/scene/Entity.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/Transform.h"
#include "engine/service/ServiceProvider.h"

/* ----- from Component -----*/

void AudioListener::OnInit(const ServiceProvider& service_provider)
{
    debug::LogInfo("{} - Init", GetName());

    audio_service_ = &service_provider.GetService<AudioService>();
    transform_ = &GetEntity().GetComponent<Transform>();

    GetEventBus().Subscribe<OnUpdateEvent>(this);
}

std::string_view AudioListener::GetName() const
{
    return "AudioListener";
}

/* ----- from EventSubscriber ----- */

void AudioListener::OnUpdate(const Timestep& delta_time)
{
    audio_service_->SetListenerPosition(transform_->GetPosition());
    audio_service_->SetListenerOrientation(transform_->GetForwardDirection(),
                                           transform_->GetUpDirection());
}
