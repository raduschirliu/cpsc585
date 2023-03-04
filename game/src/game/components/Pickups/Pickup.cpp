#include "Pickup.h"

#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"

void Pickup::OnInit(const ServiceProvider& service_provider)
{
    transform_ = &GetEntity().GetComponent<Transform>();
    game_state_ = &service_provider.GetService<GameStateService>();
}

void Pickup::OnTriggerEnter(const OnTriggerEvent& data)
{
    // // add the clause for AI here as well.
    // if (power_visible_ && data.other->GetName() == "PlayerVehicle" &&
    //     player_state_->GetCurrentPowerup() ==
    //         PowerupPickupType::kDefaultPowerup)
    // {

    // }
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
