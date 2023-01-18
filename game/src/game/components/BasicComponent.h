#pragma once

#include "engine/render/RenderService.h"
#include "engine/scene/Component.h"

class BasicComponent final : public Component
{
  public:
    BasicComponent();

    // From Component
    void Init(ComponentInitializer& initializer) override;
    std::string_view GetName() const override;
};
