#include "engine/physics/PhysicsService.h"

#include "engine/core/debug/Log.h"
#include "engine/core/math/Physx.h"
#include "engine/service/ServiceProvider.h"

#include "HelperUtils.h"

#define PVD_HOST "127.0.0.1"

using std::string_view;
using namespace physx;

// Command gCommands[] =
// {
//     {0.5f, 0.0f, 0.0f, 2.0f},		//brake on and come to rest for 2 seconds
//     {0.0f, 0.5f, 0.0f, 5.0f},		//throttle for 5 seconds
//     {0.5f, 0.0f, 0.0f, 5.0f},		//brake for 5 seconds
//     {0.0f, 0.5f, 0.0f, 5.0f},		//throttle for 5 seconds
//     {0.0f, 0.1f, 0.5f, 5.0f}		//light throttle and steer for 5 seconds.
// };

// const PxU32 gNbCommands = sizeof(gCommands) / sizeof(Command);

void PhysicsService::OnInit()
{
    Log::info("PhysicsService Initializing");

    // initializing all the physx objects for use later.
    initPhysX();

    // //
    // initMaterialFrictionTable();
    // if (!initVehicles())
    //     std::cerr << "Error In physx service.";

    // //Check that we can read from the json file before continuing.
    // BaseVehicleParams baseParams;
    // if (!readBaseParamsFromJsonFile(gVehicleDataPath, "Base.json", baseParams))
    //     Log::error("Cannot open Base.json file");

    // //Check that we can read from the json file before continuing.
    // DirectDrivetrainParams directDrivetrainParams;
    // if (!readDirectDrivetrainParamsFromJsonFile(gVehicleDataPath, "DirectDrive.json",
    //     baseParams.axleDescription, directDrivetrainParams))
    //     Log::error("Cannot open DirectDrive.json file");

}

void PhysicsService::OnStart(ServiceProvider& service_provider)
{
}

void PhysicsService::OnUpdate()
{
    // if (gNbCommands == gCommandProgress)
    //     return;

    
    // //Apply the brake, throttle and steer to the command state of the direct drive vehicle.
    // const Command& command = gCommands[gCommandProgress];
    // gVehicle.mCommandState.brakes[0] = command.brake;
    // gVehicle.mCommandState.nbBrakes = 1;
    // gVehicle.mCommandState.throttle = command.throttle;
    // gVehicle.mCommandState.steer = command.steer;

    // //Forward integrate the vehicle by a single timestep.
    // gVehicle.step(timestep, gVehicleSimulationContext);

    // Log::debug("OnUpdate() Physics, simulating at 60 fps");
    // simulate the physics
    kScene_->simulate(timestep);
    kScene_->fetchResults(true);

    //Increment the time spent on the current command.
    //Move to the next command in the list if enough time has lapsed.
    // gCommandTime += timestep;
    // if (gCommandTime > gCommands[gCommandProgress].duration)
    // {
    //     gCommandProgress++;
    //     gCommandTime = 0.0f;
    // }
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


void PhysicsService::CreatePlaneRigidBody(PxPlane plane)
{
    physx::PxRigidStatic* groundPlane = physx::PxCreatePlane(
        *kPhysics_, plane, *kMaterial_);  // now we have the plane actor.
    kScene_->addActor(*groundPlane);
}

// bool PhysicsService::initVehicles()
// {
//     //Load the params from json or set directly.
//     readBaseParamsFromJsonFile(gVehicleDataPath, "Base.json", gVehicle.mBaseParams);
//     setPhysXIntegrationParams(gVehicle.mBaseParams.axleDescription,
//         gPhysXMaterialFrictions, gNbPhysXMaterialFrictions, gPhysXDefaultMaterialFriction,
//         gVehicle.mPhysXParams);
//     readDirectDrivetrainParamsFromJsonFile(gVehicleDataPath, "DirectDrive.json",
//         gVehicle.mBaseParams.axleDescription, gVehicle.mDirectDriveParams);

//     //Set the states to default.
//     if (!gVehicle.initialize(*kPhysics_, PxCookingParams(PxTolerancesScale()), *kMaterial_))
//     {
//         return false;
//     }

//     gVehicle.mTransmissionCommandState.gear = PxVehicleDirectDriveTransmissionCommandState::eFORWARD;

//     //Apply a start pose to the physx actor and add it to the physx scene.
//     PxTransform pose(PxVec3(-5.0f, 0.5f, 0.0f), PxQuat(PxIdentity));
//     gVehicle.setUpActor(*kScene_, pose, gVehicleName);

//     //Set up the simulation context.
//     //The snippet is set up with
//     //a) z as the longitudinal axis
//     //b) x as the lateral axis
//     //c) y as the vertical axis.
//     //d) metres  as the lengthscale.
//     gVehicleSimulationContext.setToDefault();
//     gVehicleSimulationContext.frame.lngAxis = PxVehicleAxes::ePosZ;
//     gVehicleSimulationContext.frame.latAxis = PxVehicleAxes::ePosX;
//     gVehicleSimulationContext.frame.vrtAxis = PxVehicleAxes::ePosY;
//     gVehicleSimulationContext.scale.scale = 1.0f;
//     gVehicleSimulationContext.gravity = gGravity;
//     gVehicleSimulationContext.physxScene = kScene_;
//     gVehicleSimulationContext.physxActorUpdateMode = PxVehiclePhysXActorUpdateMode::eAPPLY_ACCELERATION;
//     return true;
// }

// void PhysicsService::initMaterialFrictionTable()
// {
//     //Each physx material can be mapped to a tire friction value on a per tire basis.
//     //If a material is encountered that is not mapped to a friction value, the friction value used is the specified default value.
//     //In this snippet there is only a single material so there can only be a single mapping between material and friction.
//     //In this snippet the same mapping is used by all tires.
//     gPhysXMaterialFrictions[0].friction = 1.0f;
//     gPhysXMaterialFrictions[0].material = kMaterial_;
//     gPhysXDefaultMaterialFriction = 1.0f;
//     gNbPhysXMaterialFrictions = 1;
// }

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

void PhysicsService::UpdateSphereLocation(physx::PxRigidDynamic* dynamic,
    physx::PxTransform location_transform)
{
    // so that we do not use a nullptr and break the game.
    if (dynamic)
    {
        dynamic->setGlobalPose(location_transform);
    }
    else
    {
        Log::error(
            "The dynamic pointer passed does not exist, cannot change "
            "location.");
    }
}

physx::PxRigidDynamic* PhysicsService::CreateRigidDynamic(
    const glm::vec3& position, const glm::quat& orientation, PxShape* shape)
{
    Log::debug("I was called here");
    physx::PxTransform transform = CreateTransform(position, orientation);
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

physx::PxShape* PhysicsService::CreateShapeCube(float half_x, float half_y, float half_z)
{
    return kPhysics_->createShape(PxBoxGeometry(half_x, half_y, half_z), *kMaterial_);
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
    sceneDesc.gravity =
        gGravity;  // change the gravity here.
    kDispatcher_ = physx::PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher = kDispatcher_;
    sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
    kScene_ = kPhysics_->createScene(sceneDesc);
    PxPvdSceneClient* pvdClient = kScene_->getScenePvdClient();
    if (pvdClient)
    {
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }
    // setting up the vehicle physics
    PxInitVehicleExtension(*kFoundation_);

}
