#pragma once

#include <memory>
#include <object_ptr.hpp>

#include "engine/gui/OnGuiEvent.h"
#include "engine/input/InputService.h"
#include "engine/scene/Component.h"

class GameStateService;
class Texture;
class SceneDebugService;
class AssetService;

class Setting final : public Component, public IEventSubscriber<OnGuiEvent>
{
  public:
    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    std::string_view GetName() const override;

    // From IEventSubscriber<OnGuiEvent>
    void OnGui() override;

  private:
    jss::object_ptr<InputService> input_service_;
    jss::object_ptr<GameStateService> game_state_service_;
    jss::object_ptr<SceneDebugService> scene_service_;
    jss::object_ptr<AssetService> asset_service_;

    const Texture* background_;
    const Texture* home_button_;
};