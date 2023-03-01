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
#include "VehicleCommands.h"
#include "engine/gui/OnGuiEvent.h"
#include "engine/service/Service.h"
#include "vehicle2/PxVehicleAPI.h"

class AssetService;
class InputService;
class RenderService;

class PhysicsService final : public Service,
                             public IEventSubscriber<OnGuiEvent>,
                             public physx::PxSimulationEventCallback
{
  public:
    // From Service
    void OnInit() override;
    void OnStart(ServiceProvider& service_provider) override;
    void OnSceneLoaded(Scene& scene) override;
    void OnUpdate() override;
    void OnCleanup() override;
    std::string_view GetName() const override;

    // From IEventSubscriber<OnGuiEvent>
    void OnGui() override;

  private:
    jss::object_ptr<AssetService> asset_service_;
    jss::object_ptr<InputService> input_service_;
    jss::object_ptr<RenderService> render_service_;

    physx::PxDefaultAllocator kDefaultAllocator_;
    physx::PxDefaultErrorCallback kDefaultErrorCallback_;
    physx::PxFoundation* kFoundation_ = nullptr;
    physx::PxPvd* kPvd_ = nullptr;
    physx::PxPhysics* kPhysics_ = nullptr;
    physx::PxMaterial* kMaterial_ = nullptr;
    physx::PxScene* kScene_ = nullptr;
    physx::PxDefaultCpuDispatcher* kDispatcher_ = nullptr;
    physx::PxCooking* cooking_ = nullptr;

    std::vector<physx::PxRigidDynamic*> dynamic_actors_ = {};
    bool show_debug_menu_ = false;
    bool debug_draw_ = false;

    const physx::PxF32 timestep = 1.0f / 60.0f;

    void InitPhysX();
    void DrawDebugParamWidget(const std::string& name,
                              physx::PxVisualizationParameter::Enum parameter);

  public:
    void RegisterActor(physx::PxActor* actor);
    void UnregisterActor(physx::PxActor* actor);

    /* From PxSimulationEventCallback */
    void onConstraintBreak(physx::PxConstraintInfo* constraints,
                           physx::PxU32 count) override;
    void onWake(physx::PxActor** actors, physx::PxU32 count) override;
    void onSleep(physx::PxActor** actors, physx::PxU32 count) override;
    void onContact(const physx::PxContactPairHeader& pairHeader,
                   const physx::PxContactPair* pairs,
                   physx::PxU32 nbPairs) override;
    void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override;
    void onAdvance(const physx::PxRigidBody* const* bodyBuffer,
                   const physx::PxTransform* poseBuffer,
                   const physx::PxU32 count) override;

    /*
     * Casts a ray until the nearest object or no object is hit.
     *
     * @param origin location from which we cast ray : glm::vec3
     * @param unit_dir direction of the ray casted : glm::vec3
     * @param OPTIONAL max_distance furthest reach of the ray : float
     *
     * @returns raycast_result data on the object hit by cast when a cast
     *      is successful (i.e something was hit)
     *      or nothing when a cast is unsuccessful
     */
    std::optional<RaycastData> Raycast(const glm::vec3& origin,
                                       const glm::vec3& unit_dir,
                                       float max_distance = 100000);

    physx::PxShape* CreateShape(const physx::PxGeometry& geometry);
    physx::PxRigidStatic* CreatePlaneRigidStatic(
        const physx::PxPlane& dimensions);
    physx::PxTriangleMesh* CreateTriangleMesh(const std::string& mesh_name);

    physx::PxRigidDynamic* CreateRigidDynamic(const glm::vec3& position,
                                              const glm::quat& orientation);
    physx::PxRigidStatic* CreateRigidStatic(const glm::vec3& position,
                                            const glm::quat& orientation);

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

    inline physx::PxF32 GetTimeStep()
    {
        return timestep;
    }

    const physx::PxVec3& GetGravity() const;
};
