#pragma once

#include "engine/GlobalEvent.h"
#include "engine/gui/GuiService.h"
#include "engine/render/RenderService.h"
#include "engine/scene/Component.h"
#include "engine/scene/ComponentBuilder.h"

class GuiExampleComponent final : public IComponent,
                                  public OnGuiEvent::ISubscriber
{
  public:
    GuiExampleComponent(GuiService& gui_service);

    // From IComponent
    void Init() override;
    std::string_view GetName() const override;

    // From OnGuiEvent
    void OnGui() override;
};

template <>
inline std::unique_ptr<GuiExampleComponent> ComponentBuilder::Build() const
{
    auto component = std::make_unique<GuiExampleComponent>(
        service_provider_.GetService<GuiService>());

    event_dispatcher_.Subscribe<OnGuiEvent>(component.get());

    return std::move(component);
}
