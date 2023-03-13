#include "engine/gui/GuiService.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "engine/core/debug/Log.h"
#include "engine/gui/OnGuiEvent.h"

const char* kGlslVersion = "#version 330 core";

void GuiService::OnInit()
{
    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(GetWindow().GetWindowHandle(), true);
    ImGui_ImplOpenGL3_Init(kGlslVersion);
}

void GuiService::OnStart(ServiceProvider& service_provider)
{
}

void GuiService::OnUpdate()
{
    // Render ImGui frame
    glDisable(GL_FRAMEBUFFER_SRGB);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::StyleColorsDark();
    GetEventBus().Publish<OnGuiEvent>();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GuiService::OnCleanup()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

std::string_view GuiService::GetName() const
{
    return "GuiService";
}
