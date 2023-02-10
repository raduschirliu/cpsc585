#pragma once

#include "engine/physics/PhysicsService.h"
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/Transform.h"

class RigidBodyComponent : public Component,
                           public IEventSubscriber<OnUpdateEvent>
{
  public:
    void SetMass(float mass);
    float GetMass() const;

    void SyncTransform();

    // From Component
    virtual void OnInit(const ServiceProvider& service_provider) override;
    virtual void OnUpdate(const Timestep& delta_time) override;

  protected:
    jss::object_ptr<Transform> transform_;
    jss::object_ptr<PhysicsService> physics_service_;
    physx::PxRigidDynamic* dynamic_;
};
