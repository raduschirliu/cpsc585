#include "Pickup.h"

#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"

void Pickup::OnInit(const ServiceProvider& service_provider)
{
    transform_ = &GetEntity().GetComponent<Transform>();
    game_state_ = &service_provider.GetService<GameStateService>();

    if (game_state_)
    {
        k_player_names_ = game_state_->GetPlayerStaticNames();
    }
}

void Pickup::OnStart()
{
}

void Pickup::OnTriggerEnter(const OnTriggerEvent& data)
{
}

void Pickup::OnUpdate(const Timestep& delta_time)
{
}

std::string_view Pickup::GetName() const
{
    return "";
}

void Pickup::OnTriggerExit(const OnTriggerEvent& data)
{
}

void Pickup::SetPowerVisibility(bool bValue)
{
    power_visible_ = bValue;
}

void Pickup::SetVehiclePowerup(PowerupPickupType type,
                               const OnTriggerEvent& data)
{
    player_state_ = &data.other->GetComponent<PlayerState>();
    if (player_state_)
    {
        // storing the powerup in the playerstate struct
        player_state_->SetCurrentPowerup(type);
    }
}
