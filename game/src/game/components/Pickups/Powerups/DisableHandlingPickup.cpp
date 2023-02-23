#include "DisableHandlingPickup.h"

#include "engine/core/debug/Log.h"
#include "engine/scene/Entity.h"

void DisableHandlingPickup::OnInit(const ServiceProvider& service_provider)
{
    Pickup::OnInit(service_provider);
}

void DisableHandlingPickup::OnTriggerEnter(const OnTriggerEvent& data)
{
    // do what the parent class does
    Pickup::OnTriggerEnter(data);

    // make sure to check that the data is not floor or any other object. it
    // should be only a player.
    if (data.other->GetName() == "PlayerVehicle" && power_activated_)
    {
        Log::debug("everyone handling disabled");
        power_activated_ = false;
    }
}

void DisableHandlingPickup::OnTriggerExit(const OnTriggerEvent& data)
{
    Pickup::OnTriggerExit(data);
}

std::string_view DisableHandlingPickup::GetName() const
{
    return "Disable Handling";
}