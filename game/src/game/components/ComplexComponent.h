#pragma once

#include "engine/render/RenderService.h"
#include "engine/scene/Component.h"

class ComplexComponent final : public Component
{
  public:
    ComplexComponent();

    // From Component
    void Init(ComponentInitializer& initializer) override;
    std::string_view GetName() const override;
};
