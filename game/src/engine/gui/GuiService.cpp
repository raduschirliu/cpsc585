#include "engine/gui/GuiService.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "engine/core/debug/Log.h"
#include "engine/gui/OnGuiEvent.h"

const char* kGlslVersion = "#version 330 core";

void GuiService::OnInit()
{
    Log::info("[GuiService] Initializing");

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(GetWindow().GetWindowHandle(), true);
    ImGui_ImplOpenGL3_Init(kGlslVersion);

    // TODO: these should be moved somewhere else...
    framerate_ = 0;
    prev_time_ = 0.0;
    frame_count_ = 0;
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

    GetEventBus().Publish<OnGuiEvent>();

    // TODO: This should be moved somewhere else...
    double cur_time = glfwGetTime();
    frame_count_ += 1;

    if (cur_time - prev_time_ >= 1.0)
    {
        framerate_ = frame_count_;
        frame_count_ = 0;
        prev_time_ = cur_time;
    }

    ImGui::Begin("Debug Info");
    ImGui::Text("FPS: %d", framerate_);
    ImGui::End();

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
