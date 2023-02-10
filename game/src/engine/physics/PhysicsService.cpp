#include "engine/physics/PhysicsService.h"

#include "HelperUtils.h"
#include "engine/core/debug/Log.h"
#include "engine/core/math/Physx.h"
#include "engine/input/InputService.h"
#include "engine/service/ServiceProvider.h"

#define PVD_HOST "127.0.0.1"

using std::string_view;
using namespace physx;

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

void PhysicsService::CreateRaycastFromOrigin(glm::vec3 origin,
                                             glm::vec3 unit_dir)
{
    constexpr uint32_t kZoneId = 1;
    void* profile_data = kPvd_->zoneStart("raycast", false, kZoneId);

    physx::PxReal max_distance = 100000;
    physx::PxRaycastBuffer hit;
    physx::PxVec3 px_origin = GlmToPx(origin);
    physx::PxVec3 px_unit_dir = GlmToPx(unit_dir);
    physx::PxVec3 px_endpoint = px_origin + px_unit_dir * max_distance;

    bool status = kScene_->raycast(px_origin, px_unit_dir, max_distance, hit);

    PxDebugLine line(px_origin, px_endpoint, PxDebugColor::eARGB_RED);
    kScene_->getScenePvdClient()->drawLines(&line, 1);

    if (status)
    {
        Log::debug("Hit something");
    }
    else
    {
        Log::debug("No hit");
    }

    kPvd_->zoneEnd(profile_data, "raycast", false, kZoneId);
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
