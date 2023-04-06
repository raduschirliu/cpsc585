#include "game/components/ui/MainMenu.h"

#include <imgui.h>

#include "engine/asset/AssetService.h"
#include "engine/audio/AudioService.h"
#include "engine/core/debug/Log.h"
#include "engine/core/gfx/Texture.h"
#include "engine/scene/Entity.h"
#include "engine/scene/SceneDebugService.h"
#include "game/services/GameStateService.h"

using std::make_unique;
using std::string;
using std::string_view;

void MainMenu::OnInit(const ServiceProvider& service_provider)
{
    // Dependencies
    input_service_ = &service_provider.GetService<InputService>();
    game_state_service_ = &service_provider.GetService<GameStateService>();
    scene_service_ = &service_provider.GetService<SceneDebugService>();
    asset_service_ = &service_provider.GetService<AssetService>();
    audio_service_ = &service_provider.GetService<AudioService>();

    // counter = 0;
    // logo_ = make_unique<Texture>("resources/textures/ui/logo.png");
    title_ = &asset_service_->GetTexture("menu_title");
    single_button_ = &asset_service_->GetTexture("single_button");
    multi_button_ = &asset_service_->GetTexture("multi_button");
    guide_button_ = &asset_service_->GetTexture("howToPlay_button");
    setting_button_ = &asset_service_->GetTexture("settings_button");

    // Events
    GetEventBus().Subscribe<OnGuiEvent>(this);
}

string_view MainMenu::GetName() const
{
    return "MainMenu";
}

/*
    Example of all the different ImGui controls can be found by pressing
        F1 -> Show ImGui Example
    In the debug UI, there's also a tool for seeing different styles:
        tools -> style editor
    The code for this menu is here if you wanna see how a certain part of it is
   done: thirdparty/imgui/imgui_demo.
    Documentation here: https://github.com/ocornut/imgui/wiki
 */

void MainMenu::OnGui()
{
    // Configure where the window will be placed first, since we'll make it
    // non-movable
    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoDecoration;

    ImGui::SetNextWindowPos(ImVec2(0, 0));

    // const ImVec2& screen_size = ImGui::GetIO().DisplaySize;

    // ImGui::Begin() ... ImGui::End() defines a window
    // All calls to do ImGui stuff should be between these two
    ImGui::Begin("Main title", nullptr, flags | ImGuiWindowFlags_NoInputs);
    ImGui::Image(title_->GetGuiHandle(), ImVec2(ImGui::GetIO().DisplaySize.x,
                                                ImGui::GetIO().DisplaySize.y));
    ImGui::End();

    // This works just like printf(), and has the same format specifiers: %f,
    // %s, etc.
    // string some_string = "hello again";
    // ImGui::Text("hello world: %d %s", 1234, some_string.c_str());
    // ImGui::Text("Your game window is: %fpx x %fpx", screen_size.x,
    //             screen_size.y);
    // ImGui::Image(logo_->GetGuiHandle(), ImVec2(200, 200));

    // if (ImGui::Button("a nice button"))
    // {
    //     // This if statement gets ran whenever the button is clicked
    //     counter++;
    // }
    ImGui::SetNextWindowPos(ImVec2(30, 30));
    ImGui::Begin("Setting Buttons", nullptr, flags);

    // If the FramePadding does not increase, no matter how large the rounding
    // value becomes, it does not apply
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 50.0f);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                          ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                          ImVec4(0.0f, 0.0f, 0.8f, 0.5f));
    if (ImGui::ImageButton("how to play button", guide_button_->GetGuiHandle(),
                           ImVec2(222, 49)))
    {
        scene_service_->SetActiveScene("HowToPlay");
        audio_service_->AddSource("ui_pick_01.ogg");
        audio_service_->PlaySource("ui_pick_01.ogg");
    }
    ImGui::PopStyleColor(3);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                          ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                          ImVec4(1.0f, 1.0f, 1.0f, 0.3f));
    ImGui::SameLine(0.f, ImGui::GetIO().DisplaySize.x - 480.f);
    if (ImGui::ImageButton("setting button", setting_button_->GetGuiHandle(),
                           ImVec2(163, 49)))
    {
        scene_service_->SetActiveScene("Setting");
        audio_service_->AddSource("ui_pick_01.ogg");
        audio_service_->PlaySource("ui_pick_01.ogg");
    }
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(1);

    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 370,
                                   ImGui::GetIO().DisplaySize.y - 280));
    ImGui::Begin("Play Buttons", nullptr, flags);

    ImVec2 pos = ImGui::GetCursorPos();

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 50.0f);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                          ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                          ImVec4(0.0f, 0.0f, 0.8f, 0.5f));
    if (ImGui::ImageButton("single button", single_button_->GetGuiHandle(),
                           ImVec2(308, 93)))
    {
        // Has to be the name of a scene defined near the top of GameApp.cpp
        scene_service_->SetActiveScene("Track1");
        audio_service_->AddSource("ui_pick_01.ogg");
        audio_service_->PlaySource("ui_pick_01.ogg");
    }
    ImGui::PopStyleColor(3);

    ImGui::SetCursorPos(ImVec2(pos.x, pos.y + 135));

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                          ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                          ImVec4(0.8f, 0.0f, 0.0f, 0.5f));
    if (ImGui::ImageButton("multi button", multi_button_->GetGuiHandle(),
                           ImVec2(308, 93)))
    {
        // Has to be the name of a scene defined near the top of GameApp.cpp
        // scene_service_->SetActiveScene("Track1");
        audio_service_->AddSource("ui_pick_01.ogg");
        audio_service_->PlaySource("ui_pick_01.ogg");
    }
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(1);

    // ImGui::Text("Button click counter: %d", counter);

    // Seting color for only a few elements
    // ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 1.0f, 1.0f, 1.0f));
    // ImGui::Button("useless but styled button");
    // ImGui::PopStyleColor(1);

    ImGui::StyleColorsDark();
    ImGui::End();
}