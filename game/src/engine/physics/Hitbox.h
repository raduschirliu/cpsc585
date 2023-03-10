#pragma once

#include <object_ptr.hpp>

#include "game/services/GameStateService.h"
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

    // getters + setters
    physx::PxShape* GetShape();
    glm::vec3 GetSize();
    void SetSize(const glm::vec3& size);

  private:
    // component dependencies
    jss::object_ptr<VehicleComponent> vehicle_;
    jss::object_ptr<GameStateService> game_state_service_;

    physx::PxShape* shape_;
    glm::vec3 size_;
};