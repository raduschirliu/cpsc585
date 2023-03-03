#include "Pickup.h"

#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"

void Pickup::OnInit(const ServiceProvider& service_provider)
{
    transform_ = &GetEntity().GetComponent<Transform>();
}

void Pickup::OnTriggerEnter(const OnTriggerEvent& data)
{
    // add the clause for AI here as well.
    if (power_visible_ && data.other->GetName() == "PlayerVehicle")
    {
        transform_->SetScale(glm::vec3(0.f, 0.f, 0.f));
    }
}

std::string_view Pickup::GetName() const
{
    return "";
}

void Pickup::OnTriggerExit(const OnTriggerEvent& data)
{
}

void Pickup::SetPowerVisibility(bool bValue)
{
    power_visible_ = bValue;
}
