// GLEW must be included before GLFW
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <PxPhysicsAPI.h>

#include <iostream>
#include <memory>
#include <string>

#include "engine/core/debug/Assert.h"
#include "engine/core/debug/Log.h"
#include "game/GameApp.h"
#include "main.h"

#define PVD_HOST "127.0.0.1"

using std::make_shared;
//
//static physx::PxDefaultAllocator kDefaultAllocator;
//static physx::PxDefaultErrorCallback kDefaultErrorCallback;
//static physx::PxFoundation* kFoundation = nullptr;
//static physx::PxPvd* kPvd = nullptr;
//static physx::PxPhysics* kPhysics = nullptr;
//static physx::PxMaterial* kMaterial = nullptr;
//static physx::PxScene* kScene = nullptr;
//static physx::PxDefaultCpuDispatcher* kDispatcher = nullptr;

int main()
{
    initGFLW();

    //static const physx::PxU32 frameCount = 100;
    //initPhysX();



    //// making a simple plane using physx.
    //physx::PxPlane plane = physx::PxPlane(0.f,1.f,0.f,0.f);        // n.x + d
    //physx::PxRigidStatic* groundPlane = physx::PxCreatePlane(*kPhysics, plane, *kMaterial); // now we have the plane actor.
    //kScene->addActor(*groundPlane);

    //auto sphere = physx::PxSphereGeometry(3.f);
    //physx::PxRigidDynamic* dynamic = physx::PxCreateDynamic(*kPhysics, physx::PxTransform(0.f, 0.f, 10.f), sphere, *kMaterial, 10.0f);
    //dynamic->setGlobalPose(physx::PxTransform(0.f, 109.f, 0.f));
    //dynamic->setAngularDamping(0.5f);
    //dynamic->setLinearVelocity(physx::PxVec3(0));
    //kScene->addActor(*dynamic);   

    //for (physx::PxU32 i = 0; i < frameCount * 5; i++)
    //{
    //    kScene->simulate(3.f / 60.0f);
    //    kScene->fetchResults(true);
    //}


    // Create and start app
    Log::debug("Starting app");
    auto game = make_shared<GameApp>();
    game->Run();

    // GLFW cleanup
    Log::debug("Cleaning up");
    glfwTerminate();
    return 0;
}

void initGFLW()
{
    // GFLW init
    Log::debug("Initializing GLFW");
    int glfw_status = glfwInit();
    ASSERT_MSG(glfw_status == GLFW_TRUE, "GLFW must be initialized");
}

void initPhysX()
{
//    // PhysX init
//    Log::debug("Initializing PhysX");
//    kFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, kDefaultAllocator,
//        kDefaultErrorCallback);
//    ASSERT_MSG(kFoundation, "PhysX must be initialized");
//
//    //// For debugging purposes, initializing the physx visual debugger (download: https://developer.nvidia.com/gameworksdownload#)
//    kPvd = PxCreatePvd(*kFoundation);       // create the instance of pvd
//    ASSERT_MSG(kPvd, "Error initializing PhysX Visual Debugger");
//
//    physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
//    ASSERT_MSG(transport, "Error connecting to PhysX Visual Debugger");
//
//    kPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
//
//    // Physics initlaization
//    bool recordMemoryAllocations = true;
//    kPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *kFoundation, physx::PxTolerancesScale(), recordMemoryAllocations, kPvd);
//
//    // create default material
//    kMaterial = kPhysics->createMaterial(0.5f, 0.5f, 0.6f);
//
//    physx::PxSceneDesc sceneDesc(kPhysics->getTolerancesScale());
//    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
//    kDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
//    sceneDesc.cpuDispatcher = kDispatcher;
//    sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
//    kScene = kPhysics->createScene(sceneDesc);

}
