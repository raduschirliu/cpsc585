#pragma once

#include <object_ptr.hpp>
#include <memory>

#include "engine/gui/OnGuiEvent.h"
#include "engine/input/InputService.h"
#include "engine/scene/Component.h"

class GameStateService;
class Texture;

class MainMenu final : public Component,
                                  public IEventSubscriber<OnGuiEvent>
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

    std::unique_ptr<Texture> logo_;
};
