#include "game/components/ui/HowToPlay.h"

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

void HowToPlay::OnInit(const ServiceProvider& service_provider)
{
    // Dependencies
    input_service_ = &service_provider.GetService<InputService>();
    game_state_service_ = &service_provider.GetService<GameStateService>();
    scene_service_ = &service_provider.GetService<SceneDebugService>();
    asset_service_ = &service_provider.GetService<AssetService>();
    audio_service_ = &service_provider.GetService<AudioService>();

    instruction_ = &asset_service_->GetTexture("how_to_play");
    next_button_ = &asset_service_->GetTexture("next_button");

    // Events
    GetEventBus().Subscribe<OnGuiEvent>(this);
}

string_view HowToPlay::GetName() const
{
    return "HowToPlay";
}

void HowToPlay::OnGui()
{
    // Configure where the window will be placed first, since we'll make it
    // non-movable
    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoDecoration;

    ImGui::SetNextWindowPos(ImVec2(0, 0));

    // const ImVec2& screen_size = ImGui::GetIO().DisplaySize;

    ImGui::Begin("HowToPlay", nullptr, flags | ImGuiWindowFlags_NoInputs);
    ImGui::Image(
        instruction_->GetGuiHandle(),
        ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y));
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 100,
                                   ImGui::GetIO().DisplaySize.y - 100));
    ImGui::Begin("Button", nullptr, flags);

    // If the FramePadding does not increase, no matter how large the rounding
    // value becomes, it does not apply
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 50.0f);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                          ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                          ImVec4(1.0f, 1.0f, 1.0f, 0.1f));
    if (ImGui::ImageButton("next button", next_button_->GetGuiHandle(),
                           ImVec2(40, 37)))
    {
        scene_service_->SetActiveScene("Powerups");
        audio_service_->PlaySource("ui_pick_01.ogg");
    }

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(1);
    ImGui::StyleColorsDark();

    ImGui::End();
}