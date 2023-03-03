#pragma once

#include "engine/physics/RigidBodyComponent.h"

class BoxRigidBody final : public RigidBodyComponent
{
  public:
    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    void OnDestroy() override;
    std::string_view GetName() const override;

    void SetSize(const glm::vec3& size);
    physx::PxShape* GetShape();

  private:
    physx::PxShape* shape_;
};
