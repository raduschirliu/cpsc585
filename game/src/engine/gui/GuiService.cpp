#include "engine/gui/GuiService.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

const char* kGlslVersion = "#version 330 core";

GuiService::GuiService(Window& window) : window_(window)
{
}

void GuiService::Init()
{
    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window_.GetWindowHandle(), true);
    ImGui_ImplOpenGL3_Init(kGlslVersion);
}

void GuiService::Start()
{
}

void GuiService::Update()
{
    // Render ImGui frame
    glDisable(GL_FRAMEBUFFER_SRGB);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // TODO: Render GUI
    ImGui::ShowDemoWindow(nullptr);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GuiService::Cleanup()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

std::string_view GuiService::GetName() const
{
    return "GuiService";
}
