#include "PlayerState.h"

#include <iostream>

void PlayerState::OnInit(const ServiceProvider& service_provider)
{
    GetEventBus().Subscribe<OnUpdateEvent>(this);
}

void PlayerState::OnUpdate(const Timestep& delta_time)
{
    // time elapsed in the car since game started
    player_state_.time_elapsed += delta_time.GetSeconds();
    std::cout<<*player_state_.speed << std::endl;
}

std::string_view PlayerState::GetName() const
{
    return "Player State";
}