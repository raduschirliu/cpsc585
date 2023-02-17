#pragma once
#include <ctype.h>
#include <physx/CommonVehicleFiles/SnippetVehicleHelpers.h>
#include <physx/CommonVehicleFiles/directdrivetrain/DirectDrivetrain.h>
#include <physx/CommonVehicleFiles/serialization/BaseSerialization.h>
#include <physx/CommonVehicleFiles/serialization/DirectDrivetrainSerialization.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <optional>
#include <vector>

#include "PxPhysicsAPI.h"
#include "RaycastData.h"
#include "VehicleCommands.h"  // to get enums and structures.
#include "engine/input/InputService.h"
#include "engine/service/Service.h"
#include "vehicle2/PxVehicleAPI.h"

using namespace physx;
using namespace physx::vehicle2;
using namespace snippetvehicle2;

class PhysicsService final : public Service,
                             public physx::PxSimulationEventCallback
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

    std::vector<physx::PxRigidDynamic*> dynamic_actors_ = {};

    // Gravitational acceleration
    const PxVec3 gGravity = PxVec3(0.0f, -9.81f, 0.0f);

    const PxF32 timestep = 1.f / 60.f;

  public:
    void RegisterActor(physx::PxActor* actor);
    void UnregisterActor(physx::PxActor* actor);

    /* From PxSimulationEventCallback */
    void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) override;
    void onWake(PxActor** actors, PxU32 count) override;
    void onSleep(PxActor** actors, PxU32 count) override;
    void onContact(const PxContactPairHeader& pairHeader,
                   const PxContactPair* pairs, PxU32 nbPairs) override;
    void onTrigger(PxTriggerPair* pairs, PxU32 count) override;
    void onAdvance(const PxRigidBody* const* bodyBuffer,
                   const PxTransform* poseBuffer, const PxU32 count) override;

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
     * Casts a ray until the nearest object or no object is hit.
     *
     * @param origin location from which we cast ray : glm::vec3
     * @param unit_dir direction of the ray casted : glm::vec3
     * @param OPTIONAL max_distance furthest reach of the ray : float
     *
     * @returns raycast_result data on the object hit by cast when a cast
     *    is successful (i.e something was hit) : RaycastData
     * @returns nothing when a cast is unsuccessful : std::nullopt
     */
    std::optional<RaycastData> Raycast(const glm::vec3& origin,
                                       const glm::vec3& unit_dir,
                                       float max_distance = 100000);

    /*
     * Function to make a plane based on
     * @param dimension : PxPlane
     */
    physx::PxRigidStatic* CreatePlaneRigidStatic(
        physx::PxPlane plane_dimensions);

    physx::PxRigidDynamic* CreateRigidDynamic(const glm::vec3& position,
                                              const glm::quat& orientation,
                                              physx::PxShape* shape = nullptr);

    physx::PxShape* CreateShape(const physx::PxGeometry& geometry);

    physx::PxShape* CreateShapeCube(float half_x, float half_y, float half_z);

    inline physx::PxPhysics* GetKPhysics()
    {
        return kPhysics_;
    }

    inline physx::PxMaterial* GetKMaterial()
    {
        return kMaterial_;
    }

    inline physx::PxScene* GetKScene()
    {
        return kScene_;
    }

    inline PxVec3 GetGravity()
    {
        return gGravity;
    }

    inline PxF32 GetTimeStep()
    {
        return timestep;
    }
};
