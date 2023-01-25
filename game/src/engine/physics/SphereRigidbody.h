#pragma once

#include "engine/physics/PhysicsService.h"
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/Transform.h"

class SphereRigidbody final : public Component,
                              public IEventSubscriber<OnUpdateEvent>
{
  public:
    void SetRadius(float radius);

    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    void OnUpdate() override;
    std::string_view GetName() const override;

  private:
    jss::object_ptr<Transform> transform_;
    jss::object_ptr<PhysicsService> physicsService_;
    physx::PxRigidDynamic* dynamic_;
    physx::PxShape* shape_;
};
