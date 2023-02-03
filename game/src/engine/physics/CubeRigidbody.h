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
    void SetHalfLength(float length_x, float length_y, float length_z);
    void CreateCube(float length_x, float length_y, float length_z);

    bool GetCanControl();
    void SetCanControl(bool bValue);

    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    void OnUpdate() override;
    std::string_view GetName() const override;

  private:
    jss::object_ptr<Transform> transform_;
    jss::object_ptr<PhysicsService> physicsService_;
    jss::object_ptr<InputService> input_service_;
    physx::PxRigidDynamic* dynamic_;
    physx::PxShape* shape_;
    bool b_can_control_ =
        false;  // if set to true can make the cube move using keys.
};
