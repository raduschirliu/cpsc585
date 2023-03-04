#include "KillAbilitiesPickup.h"

#include "engine/core/debug/Log.h"
#include "engine/scene/Entity.h"

void KillAbilitiesPickup::OnInit(const ServiceProvider& service_provider)
{
    Pickup::OnInit(service_provider);
}

void KillAbilitiesPickup::OnTriggerEnter(const OnTriggerEvent& data)
{
    // the pickup always collides with floor, so avoid that first
    if (data.other->GetName() == "PlayerVehicle")
    {
        player_state_ = &data.other->GetComponent<PlayerState>();
        if (player_state_)
        {
            // TODO: add the clause for AI here as well.
            if (power_visible_ && player_state_->GetCurrentPowerup() ==
                                      PowerupPickupType::kDefaultPowerup)
            {
                transform_->SetScale(glm::vec3(0.f, 0.f, 0.f));
                SetPowerVisibility(false);

                // Assigns this powerup to the player/AI who picked it up
                SetVehiclePowerup(PowerupPickupType::kKillAbilities, data);
            }
        }
    }
}

void KillAbilitiesPickup::OnTriggerExit(const OnTriggerEvent& data)
{
    Pickup::OnTriggerExit(data);
}

std::string_view KillAbilitiesPickup::GetName() const
{
    return "Kill Abilities";
}