#include "AudioEmitter.h"

#include "engine/audio/AudioService.h"
#include "engine/core/debug/Log.h"
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/Transform.h"

void AudioEmitter::AddSource(std::string file_name)
{
    file_name_ = file_name;
    audio_service_->AddSource(GetEntity().GetId(), file_name);
}

void AudioEmitter::PlaySource(std::string file_name)
{
    audio_service_->PlaySource(GetEntity().GetId(), file_name);
}

/* ----- from Component -----*/

void AudioEmitter::OnInit(const ServiceProvider& service_provider)
{
    Log::info("{} - Init", GetName());

    // service dependencies
    audio_service_ = &service_provider.GetService<AudioService>();

    // component dependencies
    transform_ = &GetEntity().GetComponent<Transform>();

    std::uint32_t entity = GetEntity().GetId();
    Log::debug("Entity {} can emit sound.", entity);

    GetEventBus().Subscribe<OnUpdateEvent>(this);
}

std::string_view AudioEmitter::GetName() const
{
    return "AudioEmitter";
}

/* ----- from IEventSubscriber ----- */

void AudioEmitter::OnUpdate(const Timestep& delta_time)
{
    glm::vec3 position = transform_->GetPosition();
    audio_service_->SetSourcePosition(GetEntity().GetId(), position);
}
