#pragma once

#include "engine/core/debug/Assert.h"
#include "engine/core/event/EventBus.h"
#include "engine/core/gfx/Window.h"
#include "engine/service/Service.h"

class GuiService final : public IService
{
  public:
    GuiService(Window& window, EventBus& event_bus);

    // From IService
    void OnInit() override;
    void OnStart() override;
    void OnUpdate() override;
    void OnCleanup() override;
    std::string_view GetName() const override;

  private:
    Window& window_;
    EventBus& event_bus_;
};
