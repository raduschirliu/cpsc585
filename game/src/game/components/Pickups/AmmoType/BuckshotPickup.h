#pragma once

#include <object_ptr.hpp>

#include "../Pickup.h"
#include "engine/scene/Component.h"
#include "engine/scene/Transform.h"

class BuckshotPickup final : public Pickup
{
  public:
    // From Component
    virtual void OnInit(const ServiceProvider& service_provider) override;
    virtual void OnTriggerEnter(const OnTriggerEvent& data) override;
    virtual void OnTriggerExit(const OnTriggerEvent& data) override;
    virtual std::string_view GetName() const override;

  private:
};
