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
    void Init() override;
    void Start() override;
    void Update() override;
    void Cleanup() override;
    std::string_view GetName() const override;

  private:
    Renderer renderer_;
};
