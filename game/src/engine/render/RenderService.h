#pragma once

#include <concepts>

#include "engine/render/Renderer.h"
#include "engine/service/Service.h"

class RenderService final : public IService
{
  public:
    RenderService(Window& window);

    void SayHi();

    // From IService
    void OnInit() override;
    void OnStart() override;
    void OnUpdate() override;
    void OnCleanup() override;
    std::string_view GetName() const override;

  private:
    Renderer renderer_;
};
