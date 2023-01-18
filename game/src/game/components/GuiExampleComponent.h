#pragma once

#include "engine/gui/GuiService.h"
#include "engine/gui/OnGuiEvent.h"
#include "engine/render/RenderService.h"
#include "engine/scene/Component.h"

class GuiExampleComponent final : public Component,
                                  public IEventSubscriber<OnGuiEvent>
{
  public:
    GuiExampleComponent();

    // From Component
    void Init(const ServiceProvider& service_provider) override;
    std::string_view GetName() const override;

    // From OnGuiEvent
    void OnGui() override;
};
