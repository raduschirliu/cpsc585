#include "PickupEveryoneSlower.h"

#include "engine/core/debug/Log.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"

void PickupEveryoneSlower::OnInit(const ServiceProvider& service_provider)
{
    transform_ = &GetEntity().GetComponent<Transform>();
}

void PickupEveryoneSlower::OnTriggerEnter(const OnTriggerEvent& data)
{
    if (power_activated_)
    {
        transform_->SetScale(glm::vec3(0.f, 0.f, 0.f));
        Log::debug("Energy pickup by {}", data.other->GetName());
        SetPowerActivated(false);
    }
}

std::string_view PickupEveryoneSlower::GetName() const
{
    return "Pickup";
}

void PickupEveryoneSlower::OnTriggerExit(const OnTriggerEvent& data)
{
}
