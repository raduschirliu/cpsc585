#pragma once

#include "engine/physics/RigidBodyComponent.h"

class SphereRigidBody final : public RigidBodyComponent
{
  public:
    void SetRadius(float radius);

    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    std::string_view GetName() const override;

  private:
    jss::object_ptr<Transform> transform_;
    jss::object_ptr<PhysicsService> physics_service_;
    physx::PxShape* shape_;
    float radius_;
};
