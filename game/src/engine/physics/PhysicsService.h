#pragma once

#include <ctype.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "PxPhysicsAPI.h"
#include "vehicle2/PxVehicleAPI.h"

#include "CommanVehicleFiles/directdrivetrain/DirectDrivetrain.h"
#include "CommanVehicleFiles/serialization/BaseSerialization.h"
#include "CommanVehicleFiles/serialization/DirectDrivetrainSerialization.h"
#include "CommanVehicleFiles/SnippetVehicleHelpers.h"

#include "engine/service/Service.h"

#include "HelperUtils.h"        // to get enums and structures.



using namespace physx;
using namespace physx::vehicle2;
using namespace snippetvehicle2;

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
    physx::PxFoundation* kFoundation_ = nullptr;
    physx::PxPvd* kPvd_ = nullptr;
    physx::PxPhysics* kPhysics_ = nullptr;
    physx::PxMaterial* kMaterial_ = nullptr;
    physx::PxScene* kScene_ = nullptr;
    physx::PxDefaultCpuDispatcher* kDispatcher_ = nullptr;


    //Gravitational acceleration
    const PxVec3 gGravity = PxVec3(0.0f, -9.81f, 0.0f);
    
    const PxF32 timestep = 1.f / 60.f;


    //A ground plane to drive on.
    PxRigidStatic* gGroundPlane = NULL;

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
    physx::PxRigidDynamic* CreateSphereRigidBody(
        physx::PxReal radius, physx::PxTransform transform_location,
        physx::PxReal density, physx::PxVec3 velocity,
        physx::PxReal angularDamping = 0.5f);

    /*
     * Function to update the location of the sphere.
     * @param dynamic object : PxRigidDynamic ptr (PxRigidDynamic*)
     * @param new location transform : PxTransform
     */
    void UpdateSphereLocation(physx::PxRigidDynamic* dynamic,
                              physx::PxTransform location_transform);

    /*
     * Function to make a plane based on
     * @param dimension : PxPlane
     */
    void CreatePlaneRigidBody(physx::PxPlane plane_dimensions);

    physx::PxRigidDynamic* CreateRigidDynamic(const glm::vec3& position,
                                              const glm::quat& orientation,
                                              physx::PxShape* shape = nullptr);

    physx::PxShape* CreateShape(const physx::PxGeometry& geometry);

    physx::PxShape* CreateShapeCube(float half_x, float half_y, float half_z);


    inline physx::PxPhysics* GetKPhysics() { return kPhysics_; }
    inline physx::PxMaterial* GetKMaterial() { return kMaterial_; }
    inline physx::PxScene* GetKScene() { return kScene_;}
    inline PxVec3 GetGravity() { return gGravity;}
    inline PxF32 GetTimeStep() { return timestep; }


};
