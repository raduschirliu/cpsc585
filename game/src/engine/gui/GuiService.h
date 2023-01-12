#pragma once

#include "engine/core/debug/Assert.h"
#include "engine/core/gfx/Window.h"
#include "engine/service/Service.h"

class GuiService final : public IService
{
  public:
    GuiService(Window& window);

    // From IService
    void Init() override;
    void Start() override;
    void Update() override;
    void Cleanup() override;
    std::string_view GetName() const override;

  private:
    Window& window_;
};

class OnGuiEventSubscriber
{
  public:
    virtual void OnGui() = 0;
};

STATIC_ASSERT_INTERFACE(OnGuiEventSubscriber);
