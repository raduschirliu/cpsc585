#pragma once

#include "engine/fwd/FwdComponents.h"
#include "engine/fwd/FwdPhysx.h"
#include "engine/fwd/FwdServices.h"
#include "engine/scene/Component.h"

class PlaneStaticBody final : public Component
{
  public:
    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    void OnDestroy() override;
    std::string_view GetName() const override;

  private:
    jss::object_ptr<PhysicsService> physics_service_;
    physx::PxRigidStatic* static_;
};
