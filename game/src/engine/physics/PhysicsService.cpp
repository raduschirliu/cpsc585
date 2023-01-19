#include "engine/physics/PhysicsService.h"

#include "engine/core/debug/Log.h"
#include "engine/service/ServiceProvider.h"

#define PVD_HOST "127.0.0.1"

using std::string_view;
using namespace physx;

void PhysicsService::OnInit()
{
    Log::info("PhysicsService Initializing");

    // initializing all the physx objects for use later.
    initPhysX();
}


void PhysicsService::initPhysX()
{
    // PhysX init
    Log::debug("Initializing PhysX object kFoundation");
    kFoundation_ = PxCreateFoundation(PX_PHYSICS_VERSION, kDefaultAllocator_,
        kDefaultErrorCallback_);
    ASSERT_MSG(kFoundation_, "PhysX must be initialized");

    //// For debugging purposes, initializing the physx visual debugger (download: https://developer.nvidia.com/gameworksdownload#)
    kPvd_ = PxCreatePvd(*kFoundation_);       // create the instance of pvd
    ASSERT_MSG(kPvd_, "Error initializing PhysX Visual Debugger");

    physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
    ASSERT_MSG(transport, "Error connecting to PhysX Visual Debugger");

    kPvd_->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

    // Physics initlaization
    bool recordMemoryAllocations = true;
    kPhysics_ = PxCreatePhysics(PX_PHYSICS_VERSION, *kFoundation_, physx::PxTolerancesScale(), recordMemoryAllocations, kPvd_);

    // create default material
    kMaterial_ = kPhysics_->createMaterial(0.5f, 0.5f, 0.6f);

    physx::PxSceneDesc sceneDesc(kPhysics_->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);              // change the gravity here.
    kDispatcher_ = physx::PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher = kDispatcher_;
    sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
    kScene_ = kPhysics_->createScene(sceneDesc);
}

void PhysicsService::OnStart(ServiceProvider& service_provider)
{
    physx::PxPlane plane = physx::PxPlane(0.f,1.f,0.f,0.f);        // n.x + d
    physx::PxRigidStatic* groundPlane = physx::PxCreatePlane(*kPhysics_, plane, *kMaterial_); // now we have the plane actor.
    kScene_->addActor(*groundPlane);

    auto sphere = physx::PxSphereGeometry(3.f);
    physx::PxRigidDynamic* dynamic = physx::PxCreateDynamic(*kPhysics_, physx::PxTransform(0.f, 100.f, 0.f), sphere, *kMaterial_, 10.0f);
    //dynamic->setGlobalPose(physx::PxTransform(0.f, 109.f, 0.f));
    dynamic->setAngularDamping(0.5f);
    dynamic->setLinearVelocity(physx::PxVec3(0));
    kScene_->addActor(*dynamic);   
}

void PhysicsService::OnUpdate()
{
   // Log::debug("OnUpdate() Physics, simulating at 60 fps");
    // simulate the physics 
    kScene_->simulate(1.f / 60.0f);
    kScene_->fetchResults(true);
}

void PhysicsService::OnCleanup()
{
}

string_view PhysicsService::GetName() const
{
    return "PhysicsService";
}
