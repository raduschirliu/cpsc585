#include "VampireBulletPickup.h"

#include "engine/core/debug/Log.h"
#include "engine/scene/Entity.h"

void VampireBulletPickup::OnInit(const ServiceProvider& service_provider)
{
    Pickup::OnInit(service_provider);
}

void VampireBulletPickup::OnTriggerEnter(const OnTriggerEvent& data)
{
    // do what the parent class does
    Pickup::OnTriggerEnter(data);
    if (data.other->GetName() == "PlayerVehicle" && power_visible_)
    {
        SetPowerVisibility(false);
        Log::debug("Bullet will deal vampire damage.");
    }
}

void VampireBulletPickup::OnTriggerExit(const OnTriggerEvent& data)
{
    Pickup::OnTriggerExit(data);
}

std::string_view VampireBulletPickup::GetName() const
{
    return "Vampire Bullet";
}