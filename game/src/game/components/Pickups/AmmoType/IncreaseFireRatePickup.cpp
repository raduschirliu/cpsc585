#include "IncreaseFireRatePickup.h"

#include "engine/core/debug/Log.h"
#include "engine/scene/Entity.h"

void IncreaseFireRatePickup::OnInit(const ServiceProvider& service_provider)
{
    Pickup::OnInit(service_provider);
}

void IncreaseFireRatePickup::OnTriggerEnter(const OnTriggerEvent& data)
{
    if (k_player_names_.find(data.other->GetName()) != k_player_names_.end())
    {
        player_state_ = &data.other->GetComponent<PlayerState>();
        if (player_state_)
        {
            if (power_visible_ && player_state_->GetCurrentAmmoType() ==
                                      AmmoPickupType::kDefaultAmmo)
            {
                transform_->SetScale(glm::vec3(0.0f, 0.0f, 0.0f));
                SetPowerVisibility(false);

                // Add the game state service here which takes care of how to
                // assign the ammo type properly
                player_state_->SetCurrentAmmoType(AmmoPickupType::kIncreaseFireRate);
            }
            else
            {
                debug::LogDebug(
                    "Ignoring as the player already as another type of ammo");
            }
        }
    }
}

void IncreaseFireRatePickup::OnTriggerExit(const OnTriggerEvent& data)
{
    Pickup::OnTriggerExit(data);
}

std::string_view IncreaseFireRatePickup::GetName() const
{
    return "Increased Fire Rate";
}