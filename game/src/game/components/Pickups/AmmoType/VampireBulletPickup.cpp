#include "VampireBulletPickup.h"

#include "engine/core/debug/Log.h"
#include "engine/scene/Entity.h"

void VampireBulletPickup::OnInit(const ServiceProvider& service_provider)
{
    Pickup::OnInit(service_provider);
}

void VampireBulletPickup::OnTriggerEnter(const OnTriggerEvent& data)
{
    if (k_player_names_.find(data.other->GetName()) != k_player_names_.end())
    {
        player_state_ = &data.other->GetComponent<PlayerState>();
        if (player_state_)
        {
            bool current_ammo = (player_state_->GetCurrentAmmoType() ==
                                     AmmoPickupType::kDefaultAmmo ||
                                 player_state_->GetCurrentAmmoType() ==
                                     AmmoPickupType::kDoubleDamage) ||
                                player_state_->GetCurrentAmmoType() ==
                                    AmmoPickupType::kIncreaseFireRate;
            if (power_visible_ && current_ammo)
            {
                transform_->SetScale(glm::vec3(0.0f, 0.0f, 0.0f));
                SetPowerVisibility(false);

                // Add the game state service here which takes care of how to
                // assign the ammo type properly
                player_state_->SetCurrentAmmoType(
                    AmmoPickupType::kVampireBullet);
            }
        }
    }
}

void VampireBulletPickup::OnTriggerExit(const OnTriggerEvent& data)
{
    Pickup::OnTriggerExit(data);
}

std::string_view VampireBulletPickup::GetName() const
{
    return "Vampire Bullet";
}