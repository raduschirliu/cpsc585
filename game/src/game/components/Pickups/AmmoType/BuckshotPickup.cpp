#include "BuckshotPickup.h"

#include "engine/core/debug/Log.h"
#include "engine/scene/Entity.h"

void BuckshotPickup::OnInit(const ServiceProvider& service_provider)
{
    Pickup::OnInit(service_provider);
}

void BuckshotPickup::OnTriggerEnter(const OnTriggerEvent& data)
{
    if (k_player_names_.find(data.other->GetName()) != k_player_names_.end())
    {
        // do what the parent class does
        Pickup::OnTriggerEnter(data);
        if (data.other->GetName() == "PlayerVehicle" && power_visible_)
        {
            SetPowerVisibility(false);
            debug::LogDebug("Buckshot Ammo Type");
        }
    }
}

void BuckshotPickup::OnTriggerExit(const OnTriggerEvent& data)
{
    Pickup::OnTriggerExit(data);
}

std::string_view BuckshotPickup::GetName() const
{
    return "Buckshot Ammo";
}