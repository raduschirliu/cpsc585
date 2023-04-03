#include "BuckshotPickup.h"

#include "engine/core/debug/Log.h"
#include "engine/scene/Entity.h"

void BuckshotPickup::OnInit(const ServiceProvider& service_provider)
{
    Pickup::OnInit(service_provider);
    GetEventBus().Subscribe<OnUpdateEvent>(this);
}

void BuckshotPickup::OnTriggerEnter(const OnTriggerEvent& data)
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
                // start the timer as soon as the powerup is picked up.
                start_timer_ = true;
                start_deactivate_timer_ = true;
                transform_->SetScale(glm::vec3(0.0f, 0.0f, 0.0f));
                SetPowerVisibility(false);

                // Add the game state service here which takes care of how
                // to assign the ammo type properly
                player_state_->SetCurrentAmmoType(AmmoPickupType::kBuckshot);
            }
        }
    }
}

void BuckshotPickup::OnUpdate(const Timestep& delta_time)
{
    Pickup::OnUpdate(delta_time);
    if (start_timer_)
    {
        timer_ += delta_time.GetSeconds();
    }
    // retrieving the Max allowed timer for the powerup from the pickupservice.
    if (timer_ >= GetMaxRespawnTime(std::string(GetName())))
    {
        start_timer_ = false;
        timer_ = 0.0f;

        transform_->SetScale(glm::vec3(4.f, 4.f, 4.f));
        SetPowerVisibility(true);
    }

    // For deactivating this powerup so that user cannot use it anymore.
    if (start_deactivate_timer_)
    {
        deactivate_timer_ += delta_time.GetSeconds();
    }
    if (deactivate_timer_ >= GetDeactivateTime(std::string(GetName())))
    {
        start_deactivate_timer_ = false;
        deactivate_timer_ = 0.0f;

        player_state_->SetCurrentAmmoType(AmmoPickupType::kDefaultAmmo);
    }
}

void BuckshotPickup::OnTriggerExit(const OnTriggerEvent& data)
{
    Pickup::OnTriggerExit(data);
}

std::string_view BuckshotPickup::GetName() const
{
    return "Buckshot";
}