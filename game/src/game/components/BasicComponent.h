#pragma once

#include "engine/scene/Component.h"

class BasicComponent final : public Component
{
  public:
    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    void OnUpdate() override;
    std::string_view GetName() const override;
};
