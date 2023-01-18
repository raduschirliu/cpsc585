#pragma once

#include "engine/scene/Component.h"

class BasicComponent final : public Component
{
  public:
    // From Component
    void Init(const ServiceProvider& service_provider) override;
    std::string_view GetName() const override;
};
