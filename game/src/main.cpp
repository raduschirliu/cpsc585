#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>
#include <memory>
#include <string>

#include "engine/ShaderManager.h"
#include "engine/gfx/GLDebug.h"
#include "engine/gfx/Geometry.h"
#include "engine/gfx/Log.h"
#include "engine/gfx/Shader.h"
#include "engine/gfx/ShaderProgram.h"
#include "engine/gfx/Texture.h"
#include "engine/gfx/Window.h"

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
    Window window(1280, 720, "CPSC 585 Game");
    GLDebug::enable();

    // Shader init
    ShaderManager::Init();

    // Scene init
    auto scene_manager = make_shared<SceneManager>();
    window.SetCallbacks(scene_manager);

    scene_manager->RegisterScene("Solar System",
                                 make_unique<SolarSystemScene>());
    scene_manager->SetActiveScene(0);
    scene_manager->OnWindowSizeChanged(window.getWidth(), window.getHeight());

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window.GetWindowHandle(), true);
    ImGui_ImplOpenGL3_Init(kGlslVersion);

    // RENDER LOOP
    while (!window.ShouldClose())
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

        window.SwapBuffers();
    }

    ShaderManager::Destroy();

    // ImGui cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}
