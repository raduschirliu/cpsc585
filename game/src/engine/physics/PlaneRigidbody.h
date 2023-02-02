#pragma once

#include "engine/physics/PhysicsService.h"
#include "engine/scene/Component.h"

class PlaneRigidbody final : public Component
{
  public:
    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    std::string_view GetName() const override;

  private:
    jss::object_ptr<PhysicsService> physicsService_;
};
