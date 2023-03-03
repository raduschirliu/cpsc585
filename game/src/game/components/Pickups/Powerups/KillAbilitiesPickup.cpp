#include "KillAbilitiesPickup.h"

#include "engine/core/debug/Log.h"
#include "engine/scene/Entity.h"

void KillAbilitiesPickup::OnInit(const ServiceProvider& service_provider)
{
    Pickup::OnInit(service_provider);
}

void KillAbilitiesPickup::OnTriggerEnter(const OnTriggerEvent& data)
{
    // do what the parent class does
    Pickup::OnTriggerEnter(data);
    if (data.other->GetName() == "PlayerVehicle" && power_visible_)
    {
        SetPowerVisibility(false);
        Log::debug("No one can use their ability anymore");
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