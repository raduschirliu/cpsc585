// GLEW must be included before GLFW

// clang-format off
#include <GL/glew.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <PxPhysicsAPI.h>

#include <iostream>
#include <memory>
#include <string>

#include "engine/core/debug/Assert.h"
#include "engine/core/debug/Log.h"
#include "engine/core/gfx/GLDebug.h"
#include "game/GameApp.h"

using std::make_shared;

static physx::PxDefaultAllocator kDefaultAllocator;
static physx::PxDefaultErrorCallback kDefaultErrorCallback;
static physx::PxFoundation* kFoundation = nullptr;

int main()
{
    // GFLW init
    Log::debug("Initializing GLFW");
    int glfw_status = glfwInit();
    ASSERT_MSG(glfw_status == GLFW_TRUE, "GLFW must be initialized");
    GLDebug::enable();

    // PhysX init
    Log::debug("Initializing PhysX");
    kFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, kDefaultAllocator,
                                     kDefaultErrorCallback);
    ASSERT_MSG(kFoundation, "PhysX must be initialized");

    // Create and start app
    Log::debug("Starting app");
    auto game = make_shared<GameApp>();
    game->Run();

    // GLFW cleanup
    Log::debug("Cleaning up");
    glfwTerminate();
    return 0;
}
