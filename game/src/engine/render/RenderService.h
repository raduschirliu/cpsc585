#pragma once

#include <concepts>

#include "engine/render/Renderer.h"
#include "engine/service/Service.h"

class RenderService final : public Service
{
  public:
    RenderService();

    void SayHi();

    // From Service
    void OnInit() override;
    void OnStart(ServiceProvider& service_provider) override;
    void OnUpdate() override;
    void OnCleanup() override;
    std::string_view GetName() const override;

  private:
    Renderer renderer_;
};
