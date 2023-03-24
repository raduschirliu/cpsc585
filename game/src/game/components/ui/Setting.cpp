#include "game/components/ui/Setting.h"

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

void Setting::OnInit(const ServiceProvider& service_provider)
{
    // Dependencies
    input_service_ = &service_provider.GetService<InputService>();
    game_state_service_ = &service_provider.GetService<GameStateService>();
    scene_service_ = &service_provider.GetService<SceneDebugService>();
    asset_service_ = &service_provider.GetService<AssetService>();
    audio_service_ = &service_provider.GetService<AudioService>();

    background_ = &asset_service_->GetTexture("background");
    home_button_ = &asset_service_->GetTexture("home_button");

    // Events
    GetEventBus().Subscribe<OnGuiEvent>(this);
}

string_view Setting::GetName() const
{
    return "Setting";
}

void Setting::OnGui()
{
    // Configure where the window will be placed first, since we'll make it
    // non-movable
    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoDecoration;

    ImGui::SetNextWindowPos(ImVec2(0, 0));

    // const ImVec2& screen_size = ImGui::GetIO().DisplaySize;

    ImGui::Begin("Setting", nullptr, flags | ImGuiWindowFlags_NoInputs);
    ImGui::Image(
        background_->GetGuiHandle(),
        ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y));
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(300, 225));
    ImGui::Begin("Options", nullptr, flags);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 50.0f);

    // Game setting elements
    static bool check = true;
    ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.f, 0.f, 0.f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.f, 1.f, 1.f, 1.f));
    ImGui::Checkbox("Music", &check);
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(1);

    // TODO: hook up audio button to where the background music plays

    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 100,
                                   ImGui::GetIO().DisplaySize.y - 100));
    ImGui::Begin("Button", nullptr, flags);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 50.0f);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.f, 1.f, 1.f, 0.1f));
    if (ImGui::ImageButton("home button", home_button_->GetGuiHandle(),
                           ImVec2(40, 37)))
    {
        scene_service_->SetActiveScene("MainMenu");
    }
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(1);

    ImGui::End();
}