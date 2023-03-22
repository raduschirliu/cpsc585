#include "game/components/ui/PlayerHud.h"

#include <imgui.h>

#include "engine/core/debug/Log.h"
#include "engine/scene/Entity.h"
#include "engine/scene/Transform.h"
#include "game/components/VehicleComponent.h"
#include "game/components/state/PlayerState.h"

using std::string;
using std::string_view;

void PlayerHud::OnInit(const ServiceProvider& service_provider)
{
    // Service dependencies
    game_state_service_ = &service_provider.GetService<GameStateService>();
    asset_service_ = &service_provider.GetService<AssetService>();

    // Component dependencies
    vehicle_ = &GetEntity().GetComponent<VehicleComponent>();
    player_state_ = &GetEntity().GetComponent<PlayerState>();

    disableHandling_ = &asset_service_->GetTexture("disable");
    everyoneSlower_ = &asset_service_->GetTexture("slower");
    IncreaseAimBox_ = &asset_service_->GetTexture("double");
    killAbilities_ = &asset_service_->GetTexture("kill");

    buckshot_ = &asset_service_->GetTexture("buckshot");
    doubleDamage_ = &asset_service_->GetTexture("damage");
    exploadingBullet_ = &asset_service_->GetTexture("exploding");
    increaseFireRate_ = &asset_service_->GetTexture("fire");
    vampireBullet_ = &asset_service_->GetTexture("vampire");

    // Events
    GetEventBus().Subscribe<OnGuiEvent>(this);
}

string_view PlayerHud::GetName() const
{
    return "PlayerHud";
}

void PlayerHud::OnGui()
{
    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse;
    ImGui::SetNextWindowPos(ImVec2(30, 30));
    ImGui::Begin("Vehicle", nullptr, flags);

    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.8f, 0.f, 0.1f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.f, 0.6f, 0.4f, 1.f));
    ImGui::ProgressBar(player_state_->GetHealth(), ImVec2(400, 30), "");
    ImGui::PopStyleColor(2);

    // ImGui::Text("Checkpoint: %d/%lu", player_state_->GetLastCheckpoint(),
    //             game_state_service_->GetNumCheckpoints());
    // if (vehicle_->GetGear() == VehicleGear::kForward)
    // {
    //     ImGui::Text("Gear: Drive");
    // }
    // else
    // {
    //     ImGui::Text("Gear: Reverse");
    // }
    // ImGui::Spacing();
    // ImGui::Text("Test printing (powerup):...");
    // ImGui::SameLine(0.f, 10.f);
    // ImGui::Image(disableHandling_->GetGuiHandle(), ImVec2(70, 70));
    // ImGui::SameLine(0.f, 10.f);
    // ImGui::Image(everyoneSlower_->GetGuiHandle(), ImVec2(70, 70));
    // ImGui::SameLine(0.f, 10.f);
    // ImGui::Image(IncreaseAimBox_->GetGuiHandle(), ImVec2(70, 70));
    // ImGui::SameLine(0.f, 10.f);
    // ImGui::Image(killAbilities_->GetGuiHandle(), ImVec2(70, 70));

    // ImGui::Text("Test printing (ammo):...");
    // ImGui::SameLine(0.f, 10.f);
    // ImGui::Image(buckshot_->GetGuiHandle(), ImVec2(70, 70));
    // ImGui::SameLine(0.f, 10.f);
    // ImGui::Image(doubleDamage_->GetGuiHandle(), ImVec2(70, 70));
    // ImGui::SameLine(0.f, 10.f);
    // ImGui::Image(exploadingBullet_->GetGuiHandle(), ImVec2(70, 70));
    // ImGui::SameLine(0.f, 10.f);
    // ImGui::Image(increaseFireRate_->GetGuiHandle(), ImVec2(70, 70));
    // ImGui::SameLine(0.f, 10.f);
    // ImGui::Image(vampireBullet_->GetGuiHandle(), ImVec2(70, 70));

    if (player_state_->GetCurrentPowerup() ==
        PowerupPickupType::kDisableHandling)
    {
        ImGui::Image(disableHandling_->GetGuiHandle(), ImVec2(70, 70));
    }
    else if (player_state_->GetCurrentPowerup() ==
             PowerupPickupType::kEveryoneSlower)
    {
        ImGui::Image(everyoneSlower_->GetGuiHandle(), ImVec2(70, 70));
    }
    else if (player_state_->GetCurrentPowerup() ==
             PowerupPickupType::kIncreaseAimBox)
    {
        ImGui::Image(IncreaseAimBox_->GetGuiHandle(), ImVec2(70, 70));
    }
    else if (player_state_->GetCurrentPowerup() ==
             PowerupPickupType::kKillAbilities)
    {
        ImGui::Image(killAbilities_->GetGuiHandle(), ImVec2(100, 100));
    }

    ImGui::SetCursorPos(ImVec2(0, 600));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.f, 0.f, 0.f, 1.f));
    ImGui::Text("LAP: %d/%lu", player_state_->GetLapsCompleted(),
                game_state_service_->GetRaceConfig().num_laps);
    ImGui::PopStyleColor();
    ImGui::SameLine(0.f, 700.f);
    ImGui::Text("%0.0f KM/H", vehicle_->GetSpeed());

    ImGui::End();

    if (player_state_->GetLapsCompleted() ==
        game_state_service_->GetRaceConfig().num_laps)
    {
        
    }
}
