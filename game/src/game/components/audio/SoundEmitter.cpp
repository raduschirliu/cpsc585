#include "SoundEmitter.h"

#include "engine/audio/AudioService.h"
#include "engine/core/debug/Log.h"
#include "engine/scene/Component.h"
#include "engine/scene/Entity.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/Transform.h"

void SoundEmitter::SetSource(std::string file_name)
{
    file_name_ = file_name;
    audio_service_->AddSource(file_name, GetEntity().GetId());
}

void SoundEmitter::PlaySource()
{
    audio_service_->PlaySource(GetEntity().GetId());
}

/* ----- from Component -----*/

void SoundEmitter::OnInit(const ServiceProvider& service_provider)
{
    debug::LogInfo("{} - Init", GetName());

    // service dependencies
    audio_service_ = &service_provider.GetService<AudioService>();

    // component dependencies
    transform_ = &GetEntity().GetComponent<Transform>();

    std::uint32_t entity = GetEntity().GetId();
    debug::LogDebug("Entity {} can emit sound.", entity);

    GetEventBus().Subscribe<OnUpdateEvent>(this);
}

std::string_view SoundEmitter::GetName() const
{
    return "SoundEmitter";
}

/* ----- from IEventSubscriber ----- */

void SoundEmitter::OnUpdate(const Timestep& delta_time)
{
    glm::vec3 position = transform_->GetPosition();
    audio_service_->SetSourcePosition(GetEntity().GetId(), position);
}
