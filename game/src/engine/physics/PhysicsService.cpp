#include "engine/physics/PhysicsService.h"

#include <optional>

#include "RaycastData.h"
#include "VehicleCommands.h"
#include "engine/asset/AssetService.h"
#include "engine/core/debug/Log.h"
#include "engine/core/math/Physx.h"
#include "engine/input/InputService.h"
#include "engine/scene/Entity.h"
#include "engine/service/ServiceProvider.h"

using std::string;
using std::string_view;
using std::vector;
using namespace physx;
using namespace physx::vehicle2;

static constexpr bool kPhysxRecordAllocations = true;
static constexpr uint32_t kPhysxCpuThreads = 2;
static constexpr const char* kPvdHost = "127.0.0.1";
static constexpr int kPvdPort = 5425;
static constexpr uint32_t kPvdTimeoutMillis = 10;

static const PxVec3 kGravity(0.0f, -9.81f, 0.0f);
// Typically speed tolerance should be gravity acceleration * 1 sec
static const PxTolerancesScale kDefaultTolerancesScale(1.0f, 9.81f);
static const PxCookingParams kDefaultPxCookingParams =
    PxCookingParams(kDefaultTolerancesScale);

/* ---------- from Service ---------- */
void PhysicsService::OnInit()
{
    InitPhysX();
}

void PhysicsService::OnStart(ServiceProvider& service_provider)
{
    asset_service_ = &service_provider.GetService<AssetService>();
    input_service_ = &service_provider.GetService<InputService>();
}

void PhysicsService::OnSceneLoaded(Scene& scene)
{
    if (kScene_)
    {
        PX_RELEASE(kScene_);
    }

    PxSceneDesc scene_desc(kPhysics_->getTolerancesScale());
    scene_desc.gravity = kGravity;
    scene_desc.cpuDispatcher = kDispatcher_;
    scene_desc.filterShader = physx::PxDefaultSimulationFilterShader;

    kScene_ = kPhysics_->createScene(scene_desc);
    kScene_->setSimulationEventCallback(this);

    PxPvdSceneClient* pvd_client = kScene_->getScenePvdClient();
    if (pvd_client)
    {
        pvd_client->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS,
                                    true);
        pvd_client->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvd_client->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES,
                                    true);
    }
}

void PhysicsService::OnUpdate()
{
    if (kScene_)
    {
        kScene_->simulate(timestep);
        kScene_->fetchResults(true);
    }
}

void PhysicsService::OnCleanup()
{
    PxCloseVehicleExtension();

    PX_RELEASE(kScene_);
    PX_RELEASE(kMaterial_);
    PX_RELEASE(kDispatcher_);
    PX_RELEASE(cooking_);
    PX_RELEASE(kPhysics_);

    if (kPvd_)
    {
        PxPvdTransport* transport = kPvd_->getTransport();
        kPvd_->release();
        transport->release();
    }

    PX_RELEASE(kFoundation_);
}

string_view PhysicsService::GetName() const
{
    return "PhysicsService";
}

PxRigidStatic* PhysicsService::CreatePlaneRigidStatic(const PxPlane& dimensions)
{
    return physx::PxCreatePlane(*kPhysics_, dimensions, *kMaterial_);
}

PxTriangleMesh* PhysicsService::CreateTriangleMesh(const string& mesh_name)
{
    // Converting our Mesh to a Px Mesh description
    const Mesh& mesh = asset_service_->GetMesh(mesh_name);

    PxTriangleMeshDesc mesh_desc;
    mesh_desc.setToDefault();

    vector<PxVec3> vertices;

    for (auto& vertex : mesh.vertices)
    {
        vertices.push_back(GlmToPx(vertex.position));
    }

    mesh_desc.triangles.count = static_cast<PxU32>(mesh.indices.size()) / 3;
    mesh_desc.triangles.data = mesh.indices.data();
    mesh_desc.triangles.stride = sizeof(uint32_t) * 3;

    mesh_desc.points.count = static_cast<PxU32>(vertices.size());
    mesh_desc.points.data = vertices.data();
    mesh_desc.points.stride = sizeof(PxVec3);

    // Cook and build a TriangleMesh
    PxDefaultMemoryOutputStream cooking_out_buffer;
    PxTriangleMeshCookingResult::Enum result;

    const bool status =
        cooking_->cookTriangleMesh(mesh_desc, cooking_out_buffer, &result);
    ASSERT_MSG(status, "Mesh cooking must succeeed");
    ASSERT_MSG(result == PxTriangleMeshCookingResult::Enum::eSUCCESS,
               "Mesh cooking must not cause warnings or errors");

    PxDefaultMemoryInputData mesh_in_buffer(cooking_out_buffer.getData(),
                                            cooking_out_buffer.getSize());
    PxTriangleMesh* triangle_mesh =
        kPhysics_->createTriangleMesh(mesh_in_buffer);
    ASSERT_MSG(triangle_mesh, "Mesh creation must succeed");

    return triangle_mesh;
}

PxRigidDynamic* PhysicsService::CreateRigidDynamic(const glm::vec3& position,
                                                   const glm::quat& orientation)
{
    const PxTransform pose = CreatePxTransform(position, orientation);
    PxRigidDynamic* rigid_dynamic = kPhysics_->createRigidDynamic(pose);
    ASSERT(rigid_dynamic);

    return rigid_dynamic;
}

PxRigidStatic* PhysicsService::CreateRigidStatic(const glm::vec3& position,
                                                 const glm::quat& orientation)
{
    const PxTransform pose = CreatePxTransform(position, orientation);
    PxRigidStatic* rigid_static = kPhysics_->createRigidStatic(pose);
    ASSERT(rigid_static);

    return rigid_static;
}

void PhysicsService::RegisterActor(PxActor* actor)
{
    ASSERT_MSG(actor, "Actor must be valid");
    kScene_->addActor(*actor);
}

void PhysicsService::UnregisterActor(PxActor* actor)
{
    ASSERT_MSG(actor, "Actor must be valid");
    kScene_->removeActor(*actor);
}

PxShape* PhysicsService::CreateShape(const physx::PxGeometry& geometry)
{
    PxShape* shape = kPhysics_->createShape(geometry, *kMaterial_, true);
    ASSERT(shape);

    return shape;
}

/* ---------- raycasting ---------- */
std::optional<RaycastData> PhysicsService::Raycast(
    const glm::vec3& origin, const glm::vec3& unit_dir,
    float max_distance /* default = 100000 */)
{
    // convert coordinates to PhysX units
    PxVec3 px_origin = GlmToPx(origin);
    PxVec3 px_unit_dir = GlmToPx(unit_dir);

    // set flags
    PxHitFlags hit_flags = PxHitFlag::eDEFAULT;

    PxRaycastBuffer raycast_result;
    kScene_->raycast(px_origin, px_unit_dir, max_distance, raycast_result);

    // check if hit successful
    if (!raycast_result.hasBlock)
    {
        Log::debug("[Raycast]:  No hit");
        return std::nullopt;
    }

    // data validity guard checks; ensure that data is available:
    if (!PxHitFlag::ePOSITION)
    {
        Log::debug("[Raycast]: Invalid Position");
        return std::nullopt;
    }

    if (!PxHitFlag::eNORMAL)
    {
        Log::debug("[Raycast]: Invalid Normal");
        return std::nullopt;
    }

    // so we don't have to do these conversions everywhere
    RaycastData result(raycast_result);
    Log::debug("[Raycast]: Hit something");

    return result;
}

/* ---------- PhysX ----------*/
void PhysicsService::InitPhysX()
{
    kFoundation_ = PxCreateFoundation(PX_PHYSICS_VERSION, kDefaultAllocator_,
                                      kDefaultErrorCallback_);
    ASSERT_MSG(kFoundation_, "PhysX must be initialized");

    kDispatcher_ = physx::PxDefaultCpuDispatcherCreate(kPhysxCpuThreads);
    ASSERT(kDispatcher_);

    //// For debugging purposes, initializing the physx visual debugger
    ///(download: https://developer.nvidia.com/gameworksdownload#)
    kPvd_ = PxCreatePvd(*kFoundation_);  // create the instance of pvd
    ASSERT_MSG(kPvd_, "Error initializing PhysX Visual Debugger");

    physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate(
        kPvdHost, kPvdPort, kPvdTimeoutMillis);
    ASSERT_MSG(transport, "Error creating PvdTransport Socket");

    const bool pvd_status =
        kPvd_->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
    if (pvd_status)
    {
        Log::info("PhysX SDK connected to PVD");
    }

    // Physics initlaization
    kPhysics_ = PxCreatePhysics(PX_PHYSICS_VERSION, *kFoundation_,
                                kDefaultTolerancesScale,
                                kPhysxRecordAllocations, kPvd_);
    ASSERT(kPhysics_);

    // create default material
    kMaterial_ = kPhysics_->createMaterial(0.5f, 0.5f, 0.6f);
    ASSERT(kMaterial_);

    cooking_ = PxCreateCooking(PX_PHYSICS_VERSION, *kFoundation_,
                               kDefaultPxCookingParams);
    ASSERT(cooking_);

    // setting up the vehicle physics
    const bool vehicle_init_status = PxInitVehicleExtension(*kFoundation_);
    ASSERT(vehicle_init_status);
}

const PxVec3& PhysicsService::GetGravity() const
{
    return kGravity;
}

/* From PxSimulationEventCallback */
void PhysicsService::onConstraintBreak(PxConstraintInfo* constraints,
                                       PxU32 count)
{
}

void PhysicsService::onWake(PxActor** actors, PxU32 count)
{
}

void PhysicsService::onSleep(PxActor** actors, PxU32 count)
{
}

void PhysicsService::onContact(const PxContactPairHeader& pair_header,
                               const PxContactPair* pairs, PxU32 pairs_count)
{
}

void PhysicsService::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        PxTriggerPair& pair = pairs[i];

        Entity* trigger_entity =
            static_cast<Entity*>(pair.triggerActor->userData);
        Entity* other_entity = static_cast<Entity*>(pair.otherActor->userData);

        ASSERT_MSG(trigger_entity,
                   "PxActor userdata must be a valid entity pointer");
        ASSERT_MSG(other_entity,
                   "PxActor userdata must be a valid entity pointer");

        OnTriggerEvent event_data = {.other = other_entity};

        bool enter = pair.status == PxPairFlag::eNOTIFY_TOUCH_FOUND;

        if (enter)
        {
            for (auto& entry : trigger_entity->GetComponents())
            {
                entry.component->OnTriggerEnter(event_data);
            }
        }
        else
        {
            for (auto& entry : trigger_entity->GetComponents())
            {
                entry.component->OnTriggerExit(event_data);
            }
        }

        event_data = {.other = trigger_entity};

        if (enter)
        {
            for (auto& entry : other_entity->GetComponents())
            {
                entry.component->OnTriggerEnter(event_data);
            }
        }
        else
        {
            for (auto& entry : other_entity->GetComponents())
            {
                entry.component->OnTriggerExit(event_data);
            }
        }
    }
}

void PhysicsService::onAdvance(const PxRigidBody* const* body_buffer,
                               const PxTransform* pose_buffer,
                               const PxU32 count)
{
}
