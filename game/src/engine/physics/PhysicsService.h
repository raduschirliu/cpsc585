#pragma once

#include "engine/service/Service.h"

class PhysicsService final : public IService
{
  public:
    // From IService
    void OnInit() override;
    void OnStart() override;
    void OnUpdate() override;
    void OnCleanup() override;
    std::string_view GetName() const override;
};
