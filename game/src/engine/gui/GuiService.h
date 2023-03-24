#pragma once

#include <imgui.h>

#include <unordered_map>

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

    ImFont* GetFont(const std::string& name);

  private:
    std::unordered_map<std::string, ImFont*> fonts_;

    void AddFont(const std::string& name, ImFont* font);
};
