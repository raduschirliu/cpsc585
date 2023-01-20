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

public:
    // all the functions which will be shared

    /*
    * Function to make a sphere collider.
    * @param radius : PxReal type
    * @param location to spawn the sphere at : PxTransform
    * @param density : PxReal
    * @param velocity : PxVec3
    * @param OPTIONAL angularDamping : PxReal
    */
    void CreateSphere(physx::PxReal radius, physx::PxTransform transform_location, physx::PxReal density, physx::PxVec3 velocity, physx::PxReal angularDamping = 0.5f);

    /*
    * Function to make a plane based on
    * @param dimension : PxPlane
    */
    void CreatePlane(physx::PxPlane plane_dimensions);
};
