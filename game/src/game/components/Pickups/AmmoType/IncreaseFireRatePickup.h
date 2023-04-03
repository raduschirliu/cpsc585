#pragma once

#include <object_ptr.hpp>

#include "../Pickup.h"
#include "engine/scene/Component.h"
#include "engine/scene/Transform.h"

class IncreaseFireRatePickup final : public Pickup
{
  public:
    // From Component
    virtual void OnInit(const ServiceProvider& service_provider) override;
    virtual void OnTriggerEnter(const OnTriggerEvent& data) override;
    virtual void OnTriggerExit(const OnTriggerEvent& data) override;
    virtual std::string_view GetName() const override;

    virtual void OnUpdate(const Timestep& delta_time) override;

  private:
    // timer_ used to deactivate the ammo powerup after some time.
    double timer_ = 0.f;
    bool start_timer_ = false;

    // deactivate_timer_ to remove the powerup from user
    double deactivate_timer_ = 0.f;
    bool start_deactivate_timer_ = false;
};
