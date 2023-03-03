#include "EveryoneSlowerPickup.h"

#include "engine/core/debug/Log.h"
#include "engine/scene/Entity.h"
#include "game/components/VehicleComponent.h"
#include "game/components/state/PlayerState.h"


void EveryoneSlowerPickup::OnInit(const ServiceProvider& service_provider)
{
    Pickup::OnInit(service_provider);
}

void EveryoneSlowerPickup::OnTriggerEnter(const OnTriggerEvent& data)
{
    // do what the parent class does
    Pickup::OnTriggerEnter(data);

    // adding new functionality.
    if (data.other->GetName() == "PlayerVehicle" && power_visible_)
    {
        SetPowerVisibility(false);
        PlayerState* v = &data.other->GetComponent<PlayerState>();
        if (v)
        {
            // changing the speed of car.
            v->SetSpeedMultiplier(0.2f);
        }
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