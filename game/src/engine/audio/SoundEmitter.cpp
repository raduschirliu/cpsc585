#include "SoundEmitter.h"

#include "engine/audio/AudioService.h"
#include "engine/core/debug/Log.h"
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/Transform.h"

void SoundEmitter::AddSource(std::string file_name)
{
    audio_service_->AddSource(file_name, GetEntity().GetId());
}

/* ----- from Component -----*/

void SoundEmitter::OnInit(const ServiceProvider& service_provider)
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

std::string_view SoundEmitter::GetName() const
{
    return "SoundEmitter";
}

/* ----- from EventSubscriber ----- */

void SoundEmitter::OnUpdate(const Timestep& delta_time)
{
}
