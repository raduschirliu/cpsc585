#include "game/components/race/Checkpoint.h"

#include "engine/core/debug/Log.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"
#include "game/components/state/PlayerState.h"

void Checkpoint::OnInit(const ServiceProvider& service_provider)
{
    game_service_ = &service_provider.GetService<GameStateService>();

    game_service_->RegisterCheckpoint(GetEntity(), this);
}

void Checkpoint::OnTriggerEnter(const OnTriggerEvent& data)
{
    if (!data.other->HasComponent<PlayerState>())
    {
        return;
    }

    game_service_->PlayerCrossedCheckpoint(*data.other, checkpoint_index_);
}

std::string_view Checkpoint::GetName() const
{
    return "Checkpoint";
}

void Checkpoint::SetCheckpointIndex(int index)
{
    checkpoint_index_ = index;
}

int Checkpoint::GetCheckpointIndex() const
{
    return checkpoint_index_;
}