#include "PlayerState.h"

#include <iostream>

void PlayerState::OnInit(const ServiceProvider& service_provider)
{
    GetEventBus().Subscribe<OnUpdateEvent>(this);
    game_state_service_ = &service_provider.GetService<GameStateService>();
}

void PlayerState::OnStart()
{
}

void PlayerState::OnUpdate(const Timestep& delta_time)
{
    if (!game_state_assigned_)
    {
        if (attached_entity_)
        {
            game_state_service_->AddPlayerDetails(attached_entity_,
                                                  player_state_);
            // so that next time we do not enter this loop and redundantly add
            // more in the player details
            game_state_assigned_ = true;
        }
    }

    // time elapsed in the car since game started
    player_state_.time_elapsed += delta_time.GetSeconds();
    // std::cout<<attached_entity_->GetName();
}

std::string_view PlayerState::GetName() const
{
    return "Player State";
}