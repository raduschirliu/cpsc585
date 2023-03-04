#include "ExploadingBulletPickup.h"

#include "engine/core/debug/Log.h"
#include "engine/scene/Entity.h"

void ExploadingBulletPickup::OnInit(const ServiceProvider& service_provider)
{
    Pickup::OnInit(service_provider);
}

void ExploadingBulletPickup::OnTriggerEnter(const OnTriggerEvent& data)
{
    // do what the parent class does
    Pickup::OnTriggerEnter(data);
    if (data.other->GetName() == "PlayerVehicle" && power_visible_)
    {
        SetPowerVisibility(false);
        Log::debug("Bullet will explode on contact, dealing more damange");
    }
}

void ExploadingBulletPickup::OnTriggerExit(const OnTriggerEvent& data)
{
    Pickup::OnTriggerExit(data);
}

std::string_view ExploadingBulletPickup::GetName() const
{
    return "Exploading Bullet";
}