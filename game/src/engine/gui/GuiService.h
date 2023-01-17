#pragma once

#include "engine/core/debug/Assert.h"
#include "engine/core/event/EventSource.h"
#include "engine/core/gfx/Window.h"
#include "engine/service/Service.h"

class GuiService final : public IService
{
  public:
    GuiService(Window& window);

    // From IService
    void OnInit() override;
    void OnStart() override;
    void OnUpdate() override;
    void OnCleanup() override;
    std::string_view GetName() const override;

    EventSource<void> on_gui_event_;

  private:
    Window& window_;
};

class IOnGuiEventSubscriber
{
  public:
    virtual void OnGui() = 0;
};

STATIC_ASSERT_INTERFACE(IOnGuiEventSubscriber);
