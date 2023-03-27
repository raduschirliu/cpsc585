#include "EveryoneSlowerPickup.h"

#include "engine/core/debug/Log.h"
#include "engine/scene/Entity.h"
#include "game/components/VehicleComponent.h"
#include "game/components/state/PlayerState.h"

void EveryoneSlowerPickup::OnInit(const ServiceProvider& service_provider)
{
    Pickup::OnInit(service_provider);
}

void EveryoneSlowerPickup::OnTriggerEnter(const OnTriggerEvent& data)
{
    // the pickup always collides with floor, so avoid that first
    if (k_player_names_.find(data.other->GetName()) != k_player_names_.end())
    {
        player_state_ = &data.other->GetComponent<PlayerState>();
        if (player_state_)
        {
            // TODO: add the clause for AI here as well.
            if (power_visible_ && player_state_->GetCurrentPowerup() ==
                                      PowerupPickupType::kDefaultPowerup)
            {
                transform_->SetScale(glm::vec3(0.0f, 0.0f, 0.0f));
                SetPowerVisibility(false);

                // Assigns this powerup to the player/AI who picked it up
                SetVehiclePowerup(PowerupPickupType::kEveryoneSlower, data);
            }
            else
            {
                debug::LogDebug(
                    "Ignoring as the player already as another powerup");
            }
        }
    }
}

void EveryoneSlowerPickup::OnTriggerExit(const OnTriggerEvent& data)
{
    Pickup::OnTriggerExit(data);
}

std::string_view EveryoneSlowerPickup::GetName() const
{
    return "Everyone Slower Pickup";
}