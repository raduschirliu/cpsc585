#pragma once

#include <object_ptr.hpp>

#include "engine/physics/RigidBodyComponent.h"
#include "engine/scene/Entity.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/Transform.h"
#include "game/components/VehicleComponent.h"

class Hitbox final : public RigidBodyComponent
{
  public:
    // from RigidBodyComponent
    void OnInit(const ServiceProvider& service_provider) override;
    std::string_view GetName() const override;
    void OnUpdate(const Timestep& delta_time) override;

    // setters
    void SetSize(const glm::vec3& size);
    // void SetFollow(Entity& entity);

  private:
    // component dependencies
    jss::object_ptr<VehicleComponent> vehicle_;
    physx::PxShape* shape_;
};