#include "IncreaseAimBoxPickup.h"

#include <iostream>

#include "engine/core/debug/Log.h"
#include "engine/scene/Entity.h"

void IncreaseAimBoxPickup::OnInit(const ServiceProvider& service_provider)
{
    Pickup::OnInit(service_provider);
    GetEventBus().Subscribe<OnUpdateEvent>(this);
}

void IncreaseAimBoxPickup::OnTriggerEnter(const OnTriggerEvent& data)
{
    if (k_player_names_.find(data.other->GetName()) != k_player_names_.end())
    {
        player_state_ = &data.other->GetComponent<PlayerState>();
        if (player_state_)
        {
            // TODO: add the clause for AI here as well.
            if (power_visible_ && player_state_->GetCurrentPowerup() ==
                                      PowerupPickupType::kDefaultPowerup)
            {
                start_timer_ = true;
                transform_->SetScale(glm::vec3(0.0f, 0.0f, 0.0f));
                SetPowerVisibility(false);

                // Assigns this powerup to the player/AI who picked it up
                SetVehiclePowerup(PowerupPickupType::kIncreaseAimBox, data);
            }
        }
    }
}

float IncreaseAimBoxPickup::GetMaxRespawnTime()
{
    return pickup_service_->GetPowerupRespawnTime(std::string(GetName()));
}

float IncreaseAimBoxPickup::GetDeactivateTime()
{
    return pickup_service_->GetPowerupDuration(std::string(GetName()));
}

void IncreaseAimBoxPickup::OnUpdate(const Timestep& delta_time)
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

void IncreaseAimBoxPickup::OnTriggerExit(const OnTriggerEvent& data)
{
    Pickup::OnTriggerExit(data);
}

std::string_view IncreaseAimBoxPickup::GetName() const
{
    return "IncreaseAimBox";
}