#pragma once

#include <object_ptr.hpp>

#include "engine/fwd/FwdComponents.h"
#include "engine/fwd/FwdServices.h"
#include "engine/gui/OnGuiEvent.h"
#include "engine/scene/Component.h"
#include "game/FwdGame.h"

class Texture;
struct ImFont;

class PlayerHud final : public Component, public IEventSubscriber<OnGuiEvent>
{
  public:
    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    std::string_view GetName() const override;

    // From IEventSubscriber<OnGuiEvent>
    void OnGui() override;

  private:
    jss::object_ptr<GameStateService> game_state_service_;
    jss::object_ptr<GuiService> gui_service_;
    jss::object_ptr<AssetService> asset_service_;

    jss::object_ptr<Transform> transform_;
    jss::object_ptr<VehicleComponent> vehicle_;
    jss::object_ptr<PlayerState> player_state_;

    const Texture* disableHandling_;
    const Texture* everyoneSlower_;
    const Texture* increaseAimBox_;
    const Texture* killAbilities_;

    const Texture* buckshot_;
    const Texture* doubleDamage_;
    const Texture* exploadingBullet_;
    const Texture* increaseFireRate_;
    const Texture* vampireBullet_;

    const Texture* minimap_;

    ImFont* font_;
};
