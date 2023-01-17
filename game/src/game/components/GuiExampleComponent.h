#pragma once

#include "engine/gui/GuiService.h"
#include "engine/render/RenderService.h"
#include "engine/scene/Component.h"
#include "engine/scene/ComponentBuilder.h"

class GuiExampleComponent final : public IComponent,
                                  public IOnGuiEventSubscriber
{
  public:
    GuiExampleComponent(GuiService& gui_service);

    // From IComponent
    void Init() override;
    std::string_view GetName() const override;

    // From IOnGuiEventSubscriber
    void OnGui() override;
};

template <>
inline std::unique_ptr<GuiExampleComponent> ComponentBuilder::Build() const
{
    return std::make_unique<GuiExampleComponent>(
        service_provider_.GetService<GuiService>());
}
