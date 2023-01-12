#pragma once

#include "engine/service/Service.h"

class PhysicsService final : public IService
{
  public:
    // From IService
    void Init() override;
    void Start() override;
    void Update() override;
    void Cleanup() override;
    std::string_view GetName() const override;
};
