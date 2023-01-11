#pragma once

#include "engine/render/RenderService.h"
#include "engine/scene/Component.h"

class TestComponent final : public IComponent
{
  public:
    // From IComponent
    void Init() override;
    std::string_view GetName() const override;
};
