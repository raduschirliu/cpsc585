#include "game/components/PlayerHud.h"

#include <imgui.h>

#include "engine/core/debug/Log.h"
#include "engine/scene/Entity.h"
#include "game/components/state/PlayerState.h"

using std::string_view;

void PlayerHud::OnInit(const ServiceProvider& service_provider)
{
    // Service dependencies
    game_state_service_ = &service_provider.GetService<GameStateService>();

    // Component dependencies
    vehicle_ = &GetEntity().GetComponent<VehicleComponent>();
    player_state_ = &GetEntity().GetComponent<PlayerState>();

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
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse;
    ImGui::SetNextWindowPos(ImVec2(40, 650));
    ImGui::Begin("Vehicle", nullptr, flags);

    ImGui::Text("Speed: %0.2f", vehicle_->GetSpeed());
    ImGui::Text("Checkpoint: %d/%lu", player_state_->GetLastCheckpoint(),
                game_state_service_->GetNumCheckpoints());
    ImGui::Text("Lap: %d/%lu", player_state_->GetLapsCompleted(),
                game_state_service_->GetRaceConfig().num_laps);

    if (vehicle_->GetGear() == VehicleGear::kForward)
    {
        ImGui::Text("Gear: Drive");
    }
    else
    {
        ImGui::Text("Gear: Reverse");
    }

    ImGui::End();
}
