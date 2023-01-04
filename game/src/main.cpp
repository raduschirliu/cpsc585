#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>
#include <memory>
#include <string>

#include "core/ShaderManager.h"
#include "core/gfx/GLDebug.h"
#include "core/gfx/Geometry.h"
#include "core/gfx/Log.h"
#include "core/gfx/Shader.h"
#include "core/gfx/ShaderProgram.h"
#include "core/gfx/Texture.h"
#include "core/gfx/Window.h"
#include "core/scene/SceneManager.h"
#include "scenes/SolarSystemScene.h"

using std::make_shared;
using std::make_unique;

const char* kGlslVersion = "#version 330 core";

int main()
{
    // GFLW init
    Log::debug("Initializing GLFW");

    if (glfwInit() != GLFW_TRUE)
    {
        Log::error("Failed to initialize GLFW");
        return 1;
    }

    // Window creation and scene init
    Window window(1280, 720, "CPSC 453 - Assignment 4");
    GLDebug::enable();

    // Shader init
    ShaderManager::Init();

    // Scene init
    auto scene_manager = make_shared<SceneManager>();
    window.setCallbacks(scene_manager);

    scene_manager->RegisterScene("Solar System",
                                 make_unique<SolarSystemScene>());
    scene_manager->SetActiveScene(0);
    scene_manager->OnWindowSizeChanged(window.getWidth(), window.getHeight());

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window.getGlfwWindowHandle(), true);
    ImGui_ImplOpenGL3_Init(kGlslVersion);

    // RENDER LOOP
    while (!window.shouldClose())
    {
        glfwPollEvents();

        // Render scene
        glEnable(GL_FRAMEBUFFER_SRGB);
        glEnable(GL_DEPTH_TEST);
        // glEnable(GL_CULL_FACE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        scene_manager->UpdateScene();
        scene_manager->RenderScene();

        // Render ImGui frame
        glDisable(GL_FRAMEBUFFER_SRGB);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        scene_manager->RenderGui();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        window.swapBuffers();
    }

    ShaderManager::Destroy();

    // ImGui cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}
