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

int main()
{
    initGFLW();

    // Create and start app
    Log::debug("Starting app");
    std::shared_ptr game = make_shared<GameApp>();
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
