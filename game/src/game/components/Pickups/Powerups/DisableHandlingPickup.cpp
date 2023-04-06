#include "DisableHandlingPickup.h"

#include "engine/core/debug/Log.h"
#include "engine/scene/Entity.h"

void DisableHandlingPickup::OnInit(const ServiceProvider& service_provider)
{
    Pickup::OnInit(service_provider);
    GetEventBus().Subscribe<OnUpdateEvent>(this);
}

void DisableHandlingPickup::OnTriggerEnter(const OnTriggerEvent& data)
{
    if (k_player_names_.find(data.other->GetName()) != k_player_names_.end())
    {
        player_state_ = &data.other->GetComponent<PlayerState>();
        if (player_state_)
        {
            if (power_visible_ && player_state_->GetCurrentPowerup() ==
                                      PowerupPickupType::kDefaultPowerup)
            {
                start_timer_ = true;
                transform_->SetScale(glm::vec3(0.0f, 0.0f, 0.0f));
                SetPowerVisibility(false);

                // Assigns this powerup to the player/AI who picked it up
                SetVehiclePowerup(PowerupPickupType::kDisableHandling, data);
            }
        }
    }
}

float DisableHandlingPickup::GetMaxRespawnTime()
{
    return pickup_service_->GetPowerupRespawnTime(std::string(GetName()));
}

float DisableHandlingPickup::GetDeactivateTime()
{
    return pickup_service_->GetPowerupDuration(std::string(GetName()));
}

void DisableHandlingPickup::OnUpdate(const Timestep& delta_time)
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

        transform_->SetScale(glm::vec3(0.12f, 0.12f, 0.12f));
        SetPowerVisibility(true);
    }
}

void DisableHandlingPickup::OnTriggerExit(const OnTriggerEvent& data)
{
    Pickup::OnTriggerExit(data);
}

std::string_view DisableHandlingPickup::GetName() const
{
    return "DisableHandling";
}