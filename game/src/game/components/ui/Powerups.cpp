#include "game/components/ui/Powerups.h"

#include <imgui.h>

#include "engine/asset/AssetService.h"
#include "engine/audio/AudioService.h"
#include "engine/core/debug/Log.h"
#include "engine/core/gfx/Texture.h"
#include "engine/input/InputService.h"
#include "engine/scene/Entity.h"
#include "engine/scene/SceneDebugService.h"
#include "game/services/GameStateService.h"

using std::make_unique;
using std::string;
using std::string_view;

void Powerups::OnInit(const ServiceProvider& service_provider)
{
    // Dependencies
    input_service_ = &service_provider.GetService<InputService>();
    game_state_service_ = &service_provider.GetService<GameStateService>();
    scene_service_ = &service_provider.GetService<SceneDebugService>();
    asset_service_ = &service_provider.GetService<AssetService>();
    audio_service_ = &service_provider.GetService<AudioService>();

    powerups_ = &asset_service_->GetTexture("powerups");
    home_button_ = &asset_service_->GetTexture("home_button");

    // Events
    GetEventBus().Subscribe<OnGuiEvent>(this);
}

string_view Powerups::GetName() const
{
    return "Powerups";
}

void Powerups::OnGui()
{
    // Configure where the window will be placed first, since we'll make it
    // non-movable
    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoDecoration;

    ImGui::SetNextWindowPos(ImVec2(0, 0));

    // const ImVec2& screen_size = ImGui::GetIO().DisplaySize;

    ImGui::Begin("Powerups", nullptr, flags | ImGuiWindowFlags_NoInputs);
    ImGui::Image(
        powerups_->GetGuiHandle(),
        ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y));
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 100,
                                   ImGui::GetIO().DisplaySize.y - 100));
    ImGui::Begin("home", nullptr, flags);

    // If the FramePadding does not increase, no matter how large the rounding
    // value becomes, it does not apply
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 50.0f);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                          ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                          ImVec4(1.0f, 1.0f, 1.0f, 0.1f));
    if (ImGui::ImageButton("home button", home_button_->GetGuiHandle(),
                           ImVec2(40, 37)))
    {
        scene_service_->SetActiveScene("MainMenu");
        audio_service_->AddSource("ui_pick_01.ogg");
        audio_service_->PlaySource("ui_pick_01.ogg");
    }

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(1);

    ImGui::End();
}