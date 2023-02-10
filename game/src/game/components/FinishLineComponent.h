#pragma once

#include "engine/scene/Component.h"
#include "engine/scene/Transform.h"

class FinishLineComponent final : public Component
{
  public:
    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    void OnTriggerEnter(const OnTriggerEvent& data) override;
    std::string_view GetName() const override;
};
