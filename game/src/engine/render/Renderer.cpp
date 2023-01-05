#include "engine/render/Renderer.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

const char* kGlslVersion = "#version 330 core";

Renderer::Renderer(const Window& window) : window_(window)
{
}

void Renderer::Init()
{
    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window_.GetWindowHandle(), true);
    ImGui_ImplOpenGL3_Init(kGlslVersion);
}

void Renderer::RenderFrame()
{
    // Render
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // TODO: Render objects

    // Render ImGui frame
    glDisable(GL_FRAMEBUFFER_SRGB);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // TODO: Render GUI
    RenderGui();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::Cleanup()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Renderer::RenderGui()
{
    ImGui::ShowDemoWindow(nullptr);
}