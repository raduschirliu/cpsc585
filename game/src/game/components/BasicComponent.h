#pragma once

#include "engine/render/RenderService.h"
#include "engine/scene/Component.h"
#include "engine/scene/ComponentBuilder.h"

class BasicComponent final : public IComponent
{
  public:
    BasicComponent();

    // From IComponent
    void Init() override;
    std::string_view GetName() const override;
};
