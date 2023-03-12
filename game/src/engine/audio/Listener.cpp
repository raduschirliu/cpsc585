#include "Listener.h"

#include "engine/audio/AudioService.h"
#include "engine/core/debug/Log.h"
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/Transform.h"

/* ----- from Component -----*/

void Listener::OnInit(const ServiceProvider& service_provider)
{
    Log::info("{} - Init", GetName());

    audio_service_ = &service_provider.GetService<AudioService>();
    transform_ = &GetEntity().GetComponent<Transform>();

    GetEventBus().Subscribe<OnUpdateEvent>(this);
}

std::string_view Listener::GetName() const
{
    return "Listener";
}

/* ----- from EventSubscriber ----- */

void Listener::OnUpdate(const Timestep& delta_time)
{
    audio_service_->SetListenerPosition(transform_->GetPosition());
    audio_service_->SetListenerOrientation(transform_->GetForwardDirection(),
                                           transform_->GetUpDirection());
}
