#pragma once

#include <object_ptr.hpp>

#include "engine/fwd/FwdComponents.h"
#include "engine/fwd/FwdServices.h"
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"

class Hitbox;

class RaycastComponent final : public Component,
                               public IEventSubscriber<OnUpdateEvent>
{
  public:
    // from component
    void OnInit(const ServiceProvider& service_provider) override;
    std::string_view GetName() const override;

    // from IEventSubscriber<OnUpdateEvent>
    void OnUpdate(const Timestep& delta_time) override;

    void Shoot();

  private:
    // service dependencies
    jss::object_ptr<PhysicsService> physics_service_;
    jss::object_ptr<InputService> input_service_;

    // component dependencies
    jss::object_ptr<Transform> transform_;
    jss::object_ptr<Hitbox> hitbox_;
};