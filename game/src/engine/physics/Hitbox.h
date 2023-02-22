#pragma once

#include <object_ptr.hpp>

#include "engine/physics/RigidBodyComponent.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/Transform.h"

class Hitbox final : public RigidBodyComponent
{
  public:
    // from Component
    void OnInit(const ServiceProvider& service_provider) override;
    std::string_view GetName() const override;

    void SetSize(const glm::vec3& size);

  private:
    physx::PxShape* shape_;
};