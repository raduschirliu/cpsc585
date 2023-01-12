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

#define PVD_HOST "127.0.0.1"

using std::make_shared;

static physx::PxDefaultAllocator kDefaultAllocator;
static physx::PxDefaultErrorCallback kDefaultErrorCallback;
static physx::PxFoundation* kFoundation = nullptr;
static physx::PxPvd* kPvd;
static physx::PxPhysics* kPhysics;

int main()
{
    // GFLW init
    Log::debug("Initializing GLFW");
    int glfw_status = glfwInit();
    ASSERT_MSG(glfw_status == GLFW_TRUE, "GLFW must be initialized");

    // PhysX init
    Log::debug("Initializing PhysX");
    kFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, kDefaultAllocator,
                                     kDefaultErrorCallback);
    ASSERT_MSG(kFoundation, "PhysX must be initialized");

    //// For debugging purposes, initializing the physx visual debugger (download: https://developer.nvidia.com/gameworksdownload#)
    kPvd = PxCreatePvd(*kFoundation);       // create the instance of pvd
    physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);   
    kPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

    std::cout << "This is working";

    // Physics initlaization
    bool recordMemoryAllocations = true;
    kPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *kFoundation, physx::PxTolerancesScale(), recordMemoryAllocations, kPvd);

    // Create and start app
    Log::debug("Starting app");
    auto game = make_shared<GameApp>();
    game->Start();

    // GLFW cleanup
    Log::debug("Cleaning up");
    glfwTerminate();
    return 0;
}
