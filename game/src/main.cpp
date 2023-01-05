// GLEW must be included before GLFW
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <memory>
#include <string>

#include "engine/core/debug/Log.h"
#include "game/GameApp.h"

using std::make_shared;

int main()
{
    // GFLW init
    Log::debug("Initializing GLFW");
    if (glfwInit() != GLFW_TRUE)
    {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    // Create and start app
    auto game = make_shared<GameApp>();
    game->Start();

    // GLFW cleanup
    glfwTerminate();
    return 0;
}
