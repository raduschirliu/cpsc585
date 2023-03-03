#include "DoubleDamagePickup.h"

#include "engine/core/debug/Log.h"
#include "engine/scene/Entity.h"

void DoubleDamagePickup::OnInit(const ServiceProvider& service_provider)
{
    Pickup::OnInit(service_provider);
}

void DoubleDamagePickup::OnTriggerEnter(const OnTriggerEvent& data)
{
    // do what the parent class does
    Pickup::OnTriggerEnter(data);
    if (data.other->GetName() == "PlayerVehicle" && power_visible_)
    {
        SetPowerVisibility(false);
        Log::debug("Bullet will deal double damage");
    }
}

void DoubleDamagePickup::OnTriggerExit(const OnTriggerEvent& data)
{
    Pickup::OnTriggerExit(data);
}

std::string_view DoubleDamagePickup::GetName() const
{
    return "Double Damage Bullet";
}