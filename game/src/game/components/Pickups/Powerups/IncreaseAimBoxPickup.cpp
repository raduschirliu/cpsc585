#include "IncreaseAimBoxPickup.h"

#include "engine/core/debug/Log.h"
#include "engine/scene/Entity.h"

void IncreaseAimBoxPickup::OnInit(const ServiceProvider& service_provider)
{
    Pickup::OnInit(service_provider);
}

void IncreaseAimBoxPickup::OnTriggerEnter(const OnTriggerEvent& data)
{
    // do what the parent class does
    Pickup::OnTriggerEnter(data);

    if (data.other->GetName() == "PlayerVehicle" && power_activated_)
    {
        power_activated_ = false;
        Log::debug("AimBox increased Pickup");
    }
}

void IncreaseAimBoxPickup::OnTriggerExit(const OnTriggerEvent& data)
{
    Pickup::OnTriggerExit(data);
}

std::string_view IncreaseAimBoxPickup::GetName() const
{
    return "Increase the aim box";
}