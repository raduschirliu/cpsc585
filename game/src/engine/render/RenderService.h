#pragma once

#include <concepts>

#include "engine/service/Service.h"

class RenderService final : public IService
{
  public:
    void SayHi();

    // From IService
    void Init() override;
    void Start() override;
    void Update() override;
    void Cleanup() override;
    std::string_view GetName() const override;
};
