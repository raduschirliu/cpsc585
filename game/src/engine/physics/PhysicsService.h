#pragma once

#include "engine/service/Service.h"
#include "PxPhysicsAPI.h"


class PhysicsService final : public Service
{
private:
    void initPhysX();

public:
    // From Service
    void OnInit() override;
    void OnStart(ServiceProvider& service_provider) override;
    void OnUpdate() override;
    void OnCleanup() override;
    std::string_view GetName() const override;

private:
    physx::PxDefaultAllocator kDefaultAllocator_;
    physx::PxDefaultErrorCallback kDefaultErrorCallback_;
    physx::PxFoundation* kFoundation_            = nullptr;
    physx::PxPvd* kPvd_                          = nullptr;
    physx::PxPhysics* kPhysics_                  = nullptr;
    physx::PxMaterial* kMaterial_                = nullptr;
    physx::PxScene* kScene_                      = nullptr;
    physx::PxDefaultCpuDispatcher* kDispatcher_  = nullptr;
};
