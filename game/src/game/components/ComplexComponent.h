#pragma once

#include "engine/render/RenderService.h"
#include "engine/scene/Component.h"
#include "engine/scene/ComponentBuilder.h"

class ComplexComponent final : public IComponent
{
  public:
    ComplexComponent(RenderService& render_service);

    // From IComponent
    void Init() override;
    std::string_view GetName() const override;
};

template <>
inline std::unique_ptr<ComplexComponent> ComponentBuilder::Build() const
{
    return std::make_unique<ComplexComponent>(
        service_provider_.GetService<RenderService>());
}
