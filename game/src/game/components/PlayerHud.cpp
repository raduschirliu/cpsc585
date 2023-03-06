#include "game/components/PlayerHud.h"

#include <imgui.h>

#include "engine/core/debug/Log.h"
#include "engine/scene/Entity.h"

using std::string_view;

void PlayerHud::OnInit(const ServiceProvider& service_provider)
{
    // Dependencies
    vehicle_ = &GetEntity().GetComponent<VehicleComponent>();

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
