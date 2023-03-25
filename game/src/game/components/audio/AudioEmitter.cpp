#include "AudioEmitter.h"

#include "engine/core/debug/Log.h"
#include "engine/scene/Entity.h"

void AudioEmitter::AddSource(std::string file_name)
{
    debug::LogDebug("Entity: {} just added a source for {}",
                    GetEntity().GetId(), file_name);
    file_name_ = file_name;
    audio_service_->AddSource(GetEntity().GetId(), file_name);
}

void AudioEmitter::PlaySource(std::string file_name)
{
    debug::LogDebug("Entity: {} is playing {}", GetEntity().GetId(), file_name);
    audio_service_->PlaySource(GetEntity().GetId(), file_name);
}

void AudioEmitter::SetPitch(std::string file_name, float pitch_offset)
{
    audio_service_->SetPitch(GetEntity().GetId(), file_name, pitch_offset);
}

void AudioEmitter::SetGain(std::string file_name, float gain)
{
    audio_service_->SetGain(GetEntity().GetId(), file_name, gain);
}

void AudioEmitter::SetLoop(std::string file_name, bool is_looping)
{
    audio_service_->SetLooping(GetEntity().GetId(), file_name, is_looping);
}

/* ----- from Component -----*/

void AudioEmitter::OnInit(const ServiceProvider& service_provider)
{
    debug::LogInfo("{} - Init", GetName());

    // service dependencies
    audio_service_ = &service_provider.GetService<AudioService>();

    // component dependencies
    transform_ = &GetEntity().GetComponent<Transform>();

    uint32_t entity = GetEntity().GetId();
    debug::LogDebug("Entity {} can emit sound.", entity);

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
