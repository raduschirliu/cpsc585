#pragma once

#include <imgui.h>

#include "engine/core/debug/Assert.h"
#include "engine/core/event/EventBus.h"
#include "engine/core/gfx/Window.h"
#include "engine/service/Service.h"

class GuiService final : public Service
{
  public:
    // From Service
    void OnInit() override;
    void OnStart(ServiceProvider& service_provider) override;
    void OnUpdate() override;
    void OnCleanup() override;
    std::string_view GetName() const override;
};
