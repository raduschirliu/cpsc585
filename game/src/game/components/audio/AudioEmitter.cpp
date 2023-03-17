#include "SoundEmitter.h"

#include "engine/audio/AudioService.h"
#include "engine/core/debug/Log.h"
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/Transform.h"

void AudioEmitter::SetSource(std::string file_name)
{
    file_name_ = file_name;
    audio_service_->AddSource(file_name, GetEntity().GetId());
}

void AudioEmitter::PlaySource()
{
    audio_service_->PlaySource(GetEntity().GetId());
}

void AudioEmitter::SetPitch(float pitch_offset)
{
    audio_service_->SetPitch(pitch_offset, GetEntity().GetId());
}

void AudioEmitter::SetGain(float gain)
{
    audio_service_->SetGain(gain, GetEntity().GetId());
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
