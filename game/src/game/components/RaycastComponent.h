#pragma once

#include <object_ptr.hpp>

#include "engine/input/InputService.h"
#include "engine/physics/BoxRigidBody.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/Transform.h"

class RaycastComponent final : public Component,
                               public IEventSubscriber<OnUpdateEvent>
{
  public:
    // from component
    void OnInit(const ServiceProvider& service_provider) override;
    std::string_view GetName() const override;

    // from IEventSubscriber<OnUpdateEvent>
    void OnUpdate(const Timestep& delta_time) override;

  private:
    // service dependencies
    jss::object_ptr<InputService> input_service_;
    jss::object_ptr<PhysicsService> physics_service_;

    // component dependencies
    jss::object_ptr<Transform> transform_;
    jss::object_ptr<BoxRigidBody> box_rigid_body_;

    // the shape of the currernt entity
    physx::PxShape* self_shape_;
};