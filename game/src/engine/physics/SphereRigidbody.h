#pragma once

#include "engine/physics/PhysicsService.h"
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/Transform.h"

class SphereRigidBody final : public Component,
                              public IEventSubscriber<OnUpdateEvent>
{
  public:
    void SetRadius(float radius);

    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    std::string_view GetName() const override;

    // From IEventSubscriber<OnUpdateEvent>
    void OnUpdate(const Timestep& delta_time) override;

  private:
    jss::object_ptr<Transform> transform_;
    jss::object_ptr<PhysicsService> physicsService_;
    physx::PxRigidDynamic* dynamic_;
    physx::PxShape* shape_;
};
