// this is the basic pickup class which will be inherited by the future children
// classes in order to implement the functionality and add meshes to them

#pragma once

#include <object_ptr.hpp>

#include "../Pickup.h"
#include "engine/scene/Component.h"
#include "engine/scene/Transform.h"

class ExploadingBulletPickup final : public Pickup
{
  public:
    // From Component
    virtual void OnInit(const ServiceProvider& service_provider) override;
    virtual void OnTriggerEnter(const OnTriggerEvent& data) override;
    virtual void OnTriggerExit(const OnTriggerEvent& data) override;
    virtual std::string_view GetName() const override;
    virtual void OnUpdate(const Timestep& delta_time) override;
float GetMaxRespawnTime() override;
    float GetDeactivateTime() override;
  private:
    // timer_ used to deactivate the ammo powerup after some time.
    double timer_ = 0.f;
    bool start_timer_ = false;

    // deactivate_timer_ to remove the powerup from user
    double deactivate_timer_ = 0.f;
    bool start_deactivate_timer_ = false;
};
