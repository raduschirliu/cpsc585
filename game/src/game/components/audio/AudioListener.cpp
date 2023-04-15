#include "AudioListener.h"

#include "engine/core/debug/Log.h"
#include "engine/scene/Component.h"
#include "engine/scene/Entity.h"
#include "engine/scene/Transform.h"

/* ----- from Component -----*/

void AudioListener::OnInit(const ServiceProvider& service_provider)
{
    debug::LogInfo("{} - Init", GetName());

    audio_service_ = &service_provider.GetService<AudioService>();
    audio_service_->SetListener(GetEntity());
}

std::string_view AudioListener::GetName() const
{
    return "AudioListener";
}