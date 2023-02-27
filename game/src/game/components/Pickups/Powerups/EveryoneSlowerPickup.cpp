#include "EveryoneSlowerPickup.h"

#include "engine/core/debug/Log.h"
#include "engine/scene/Entity.h"

void EveryoneSlowerPickup::OnInit(const ServiceProvider& service_provider)
{
    Pickup::OnInit(service_provider);
}

void EveryoneSlowerPickup::OnTriggerEnter(const OnTriggerEvent& data)
{
    // do what the parent class does
    Pickup::OnTriggerEnter(data);
    if (data.other->GetName() == "PlayerVehicle" && power_activated_)
    {
        power_activated_ = false;
        Log::debug("everyone slow for 5 seconds");
    }
}

void EveryoneSlowerPickup::OnTriggerExit(const OnTriggerEvent& data)
{
    Pickup::OnTriggerExit(data);
}

std::string_view EveryoneSlowerPickup::GetName() const
{
    return "Everyone Slower Pickup";
}