#include "IncreaseFireRatePickup.h"

#include "engine/core/debug/Log.h"
#include "engine/scene/Entity.h"

void IncreaseFireRatePickup::OnInit(const ServiceProvider& service_provider)
{
    Pickup::OnInit(service_provider);
    GetEventBus().Subscribe<OnUpdateEvent>(this);
}

void IncreaseFireRatePickup::OnTriggerEnter(const OnTriggerEvent& data)
{
    if (k_player_names_.find(data.other->GetName()) != k_player_names_.end())
    {
        player_state_ = &data.other->GetComponent<PlayerState>();
        if (player_state_)
        {
            bool current_ammo = (player_state_->GetCurrentAmmoType() ==
                                     AmmoPickupType::kDefaultAmmo ||
                                 player_state_->GetCurrentAmmoType() ==
                                     AmmoPickupType::kBuckshot) ||
                                player_state_->GetCurrentAmmoType() ==
                                    AmmoPickupType::kExploadingBullet ||
                                player_state_->GetCurrentAmmoType() ==
                                    AmmoPickupType::kVampireBullet;
            if (power_visible_ && current_ammo)
            {
                start_timer_ = true;
                transform_->SetScale(glm::vec3(0.0f, 0.0f, 0.0f));
                SetPowerVisibility(false);

                // Add the game state service here which takes care of how to
                // assign the ammo type properly
                player_state_->SetCurrentAmmoType(
                    AmmoPickupType::kIncreaseFireRate);
            }
        }
    }
}

float IncreaseFireRatePickup::GetMaxRespawnTime()
{
    return pickup_service_->GetAmmoRespawnTime(std::string(GetName()));
}

float IncreaseFireRatePickup::GetDeactivateTime()
{
    return pickup_service_->GetAmmoDuration(std::string(GetName()));
}

void IncreaseFireRatePickup::OnUpdate(const Timestep& delta_time)
{
    Pickup::OnUpdate(delta_time);
    if (start_timer_)
    {
        timer_ += delta_time.GetSeconds();
    }

    // retrieving the Max allowed timer for the powerup from the pickupservice.
    if (timer_ >= GetMaxRespawnTime())
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
    if (deactivate_timer_ >= GetDeactivateTime())
    {
        start_deactivate_timer_ = false;
        deactivate_timer_ = 0.0f;

        if (player_state_->GetCurrentAmmoType() != AmmoPickupType::kDefaultAmmo)
        {
            player_state_->SetCurrentAmmoType(AmmoPickupType::kDefaultAmmo);
        }
    }
}

void IncreaseFireRatePickup::OnTriggerExit(const OnTriggerEvent& data)
{
    Pickup::OnTriggerExit(data);
}

std::string_view IncreaseFireRatePickup::GetName() const
{
    return "IncreaseFireRate";
}