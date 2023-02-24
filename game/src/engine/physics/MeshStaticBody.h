#pragma once

#include <optional>

#include "engine/physics/PhysicsService.h"
#include "engine/scene/Component.h"
#include "engine/scene/Transform.h"

class MeshStaticBody final : public Component
{
  public:
    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    void OnDestroy() override;
    std::string_view GetName() const override;

    void SetMesh(const std::string& name, float scale);

  private:
    jss::object_ptr<PhysicsService> physics_service_;

    jss::object_ptr<Transform> transform_;

    physx::PxRigidStatic* static_;
    physx::PxShape* shape_;
    std::optional<std::string> mesh_name_;
};
