#include "DisableHandlingPickup.h"

#include "engine/core/debug/Log.h"
#include "engine/scene/Entity.h"

void DisableHandlingPickup::OnInit(const ServiceProvider& service_provider)
{
    Pickup::OnInit(service_provider);
}

void DisableHandlingPickup::OnTriggerEnter(const OnTriggerEvent& data)
{
    if (data.other->GetName() == "PlayerVehicle")
    {
        player_state_ = &data.other->GetComponent<PlayerState>();
        if (player_state_)
        {  // make sure to check that the data is not floor or any other object.
           // it
            // should be only a player.
            if (power_visible_ && player_state_->GetCurrentPowerup() ==
                                      PowerupPickupType::kDefaultPowerup)
            {
                transform_->SetScale(glm::vec3(0.0f, 0.0f, 0.0f));
                SetPowerVisibility(false);

                // Assigns this powerup to the player/AI who picked it up
                SetVehiclePowerup(PowerupPickupType::kDisableHandling, data);
            }
            else
            {
                debug::LogDebug(
                    "Ignoring as the player already as another powerup");
            }
        }
    }
}

void DisableHandlingPickup::OnTriggerExit(const OnTriggerEvent& data)
{
    Pickup::OnTriggerExit(data);
}

std::string_view DisableHandlingPickup::GetName() const
{
    return "Disable Handling";
}