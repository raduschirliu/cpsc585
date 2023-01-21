#pragma once

#include "engine/gui/GuiService.h"
#include "engine/gui/OnGuiEvent.h"
#include "engine/input/InputService.h"
#include "engine/render/RenderService.h"
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"

class GuiExampleComponent final : public Component,
                                  public IEventSubscriber<OnGuiEvent>,
                                  public IEventSubscriber<OnUpdateEvent>
{
  public:
    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    std::string_view GetName() const override;

    // From IEventSubscriber<OnUpdateEvent>
    void OnUpdate() override;

    // From IEventSubscriber<OnGuiEvent>
    void OnGui() override;

  private:
    InputService* input_service_;
};
