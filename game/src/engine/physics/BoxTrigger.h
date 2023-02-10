#pragma once

#include "engine/physics/RigidBodyComponent.h"

class BoxTrigger final : public RigidBodyComponent
{
  public:
    void SetSize(const glm::vec3& size);

    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    std::string_view GetName() const override;

  private:
    physx::PxShape* shape_;
};
