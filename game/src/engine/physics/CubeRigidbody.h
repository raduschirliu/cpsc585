#pragma once

#include "engine/input/InputService.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/Transform.h"

class CubeRigidbody final : public Component,
                            public IEventSubscriber<OnUpdateEvent>
{
  public:
    void SetSize(const glm::vec3& size);

    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    void OnUpdate(const Timestep& delta_time) override;
    std::string_view GetName() const override;

  private:
    jss::object_ptr<Transform> transform_;
    jss::object_ptr<PhysicsService> physics_service_;
    jss::object_ptr<InputService> input_service_;
    physx::PxRigidDynamic* dynamic_;
    physx::PxShape* shape_;
};
