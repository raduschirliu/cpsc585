#include "engine/physics/PhysicsService.h"

#include <optional>

#include "RaycastData.h"
#include "VehicleCommands.h"
#include "engine/core/debug/Log.h"
#include "engine/core/math/Physx.h"
#include "engine/input/InputService.h"
#include "engine/scene/Entity.h"
#include "engine/service/ServiceProvider.h"

#define PVD_HOST "127.0.0.1"

using std::string_view;
using namespace physx;

/* ---------- from Service ---------- */
void PhysicsService::OnInit()
{
    // initializing all the physx objects for use later.
    initPhysX();
}

void PhysicsService::OnStart(ServiceProvider& service_provider)
{
}

void PhysicsService::OnUpdate()
{
    kScene_->simulate(timestep);
    kScene_->fetchResults(true);
}

void PhysicsService::OnCleanup()
{
    PxCloseVehicleExtension();

    PX_RELEASE(kMaterial_);
    PX_RELEASE(kScene_);
    PX_RELEASE(kDispatcher_);
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

PxRigidStatic* PhysicsService::CreatePlaneRigidStatic(PxPlane plane_dimensions)
{
    return physx::PxCreatePlane(*kPhysics_, plane_dimensions, *kMaterial_);
}

PxRigidDynamic* PhysicsService::CreateSphereRigidBody(
    PxReal radius, PxTransform transform_location, PxReal density,
    PxVec3 velocity, PxReal angularDamping)
{
    physx::PxSphereGeometry sphere = physx::PxSphereGeometry(radius);
    PxRigidDynamic* dynamic = kPhysics_->createRigidDynamic(transform_location);
    PxShape* shape = kPhysics_->createShape(sphere, *kMaterial_);
    dynamic->attachShape(*shape);

    // physx::PxRigidDynamic* dynamic = physx::PxCreateDynamic(*kPhysics_,
    // transform_location, sphere, *kMaterial_, density);
    if (dynamic)
    {
        dynamic->setAngularDamping(angularDamping);
        dynamic->setLinearVelocity(velocity);
        kScene_->addActor(*dynamic);
    }
    else
    {
        Log::error("Error occured while creating the sphere.");
    }

    PxSphereGeometry bigger_sphere(10.0f);
    PxShape* bigger_shape = kPhysics_->createShape(bigger_sphere, *kMaterial_);
    dynamic->detachShape(*shape);
    dynamic->attachShape(*bigger_shape);

    return dynamic;
}

void PhysicsService::RegisterActor(PxActor* actor)
{
    kScene_->addActor(*actor);
}

void PhysicsService::UnregisterActor(PxActor* actor)
{
    kScene_->removeActor(*actor);
}

physx::PxRigidDynamic* PhysicsService::CreateRigidDynamic(
    const glm::vec3& position, const glm::quat& orientation, PxShape* shape)
{
    physx::PxTransform transform = CreatePxTransform(position, orientation);
    physx::PxRigidDynamic* dynamic = kPhysics_->createRigidDynamic(transform);
    if (shape)
    {
        dynamic->attachShape(*shape);
        PxRigidBodyExt::updateMassAndInertia(*dynamic, 10.f);
    }
    kScene_->addActor(*dynamic);

    return dynamic;
}

physx::PxShape* PhysicsService::CreateShape(const physx::PxGeometry& geometry)
{
    return kPhysics_->createShape(geometry, *kMaterial_);
}

physx::PxShape* PhysicsService::CreateShapeCube(float half_x, float half_y,
                                                float half_z)
{
    return kPhysics_->createShape(PxBoxGeometry(half_x, half_y, half_z),
                                  *kMaterial_);
}

/* ---------- raycasting ---------- */
std::optional<RaycastData> PhysicsService::Raycast(
    const glm::vec3& origin, const glm::vec3& unit_dir,
    float max_distance /* = 100000 */)
{
    // convert coordinates
    PxVec3 px_origin = GlmToPx(origin);
    PxVec3 px_unit_dir = GlmToPx(unit_dir);

    // setting up raycast object filtering
    PxHitFlags hit_flags = PxHitFlag::eDEFAULT;
    PxQueryFlag::Enum query_flag = PxQueryFlag::ePOSTFILTER;
    PxQueryFilterData filter_data = PxQueryFilterData(query_flag);

    PxRaycastBuffer raycast_result;
    kScene_->raycast(px_origin, px_unit_dir, max_distance, raycast_result,
                     hit_flags, filter_data);

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
    Log::debug("[Raycast]: Hit something");  // pog

    return result;
}

PxQueryHitType::Enum postFilter(const PxFilterData& filter_data,
                                const PxQueryHit& hit, const PxShape* shape,
                                const PxRigidActor* actor,
                                PxHitFlags& query_flags)
{
    // if (actor == the actor that shot the raycast)
    return PxQueryHitType::Enum::eNONE;  // i.e only registers blocking hits
}

/* ---------- PhysX ----------*/
void PhysicsService::initPhysX()
{
    // PhysX init
    // Log::debug("Initializing PhysX object kFoundation");
    kFoundation_ = PxCreateFoundation(PX_PHYSICS_VERSION, kDefaultAllocator_,
                                      kDefaultErrorCallback_);
    ASSERT_MSG(kFoundation_, "PhysX must be initialized");

    //// For debugging purposes, initializing the physx visual debugger
    ///(download: https://developer.nvidia.com/gameworksdownload#)
    kPvd_ = PxCreatePvd(*kFoundation_);  // create the instance of pvd
    ASSERT_MSG(kPvd_, "Error initializing PhysX Visual Debugger");

    physx::PxPvdTransport* transport =
        physx::PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
    ASSERT_MSG(transport, "Error connecting to PhysX Visual Debugger");

    kPvd_->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

    // Physics initlaization
    bool recordMemoryAllocations = true;
    kPhysics_ = PxCreatePhysics(PX_PHYSICS_VERSION, *kFoundation_,
                                physx::PxTolerancesScale(),
                                recordMemoryAllocations, kPvd_);

    // create default material
    kMaterial_ = kPhysics_->createMaterial(0.5f, 0.5f, 0.6f);

    physx::PxSceneDesc sceneDesc(kPhysics_->getTolerancesScale());
    sceneDesc.gravity = gGravity;  // change the gravity here.
    kDispatcher_ = physx::PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher = kDispatcher_;
    sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
    kScene_ = kPhysics_->createScene(sceneDesc);
    kScene_->setSimulationEventCallback(this);
    PxPvdSceneClient* pvdClient = kScene_->getScenePvdClient();
    if (pvdClient)
    {
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES,
                                   true);
    }
    // setting up the vehicle physics
    PxInitVehicleExtension(*kFoundation_);
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
