#pragma once

#include "engine/gui/GuiService.h"
#include "engine/gui/OnGuiEvent.h"
#include "engine/render/RenderService.h"
#include "engine/scene/Component.h"
#include "engine/input/InputService.h"

class GuiExampleComponent final : public Component,
                                  public IEventSubscriber<OnGuiEvent>
{
  public:
    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    void OnUpdate() override;
    std::string_view GetName() const override;

    // From IEventSubscriber<OnGuiEvent>
    void OnGui() override;
  
  private:
    InputService* input_service_;
};
