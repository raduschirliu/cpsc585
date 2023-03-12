#include "game/components/ui/MainMenu.h"

#include <imgui.h>

#include "engine/core/debug/Log.h"
#include "engine/core/gfx/Texture.h"
#include "engine/scene/Entity.h"
#include "game/services/GameStateService.h"

using std::make_unique;
using std::string;
using std::string_view;

void MainMenu::OnInit(const ServiceProvider& service_provider)
{
    // Dependencies
    input_service_ = &service_provider.GetService<InputService>();
    game_state_service_ = &service_provider.GetService<GameStateService>();

    logo_ = make_unique<Texture>("resources/textures/ui/logo.png",
                                 Texture::InterpolationMode::kLinear);

    // Events
    GetEventBus().Subscribe<OnGuiEvent>(this);
}

string_view MainMenu::GetName() const
{
    return "MainMenu";
}

void MainMenu::OnGui()
{
    ImGui::Begin("Main menu");

    ImGui::Text("hello world");
    ImGui::Button("useless button");

    ImGui::Image(logo_->GetHandle().ValueRaw(), ImVec2(200, 200));

    ImGui::End();
}
