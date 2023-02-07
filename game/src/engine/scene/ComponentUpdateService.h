#pragma once

#include "engine/core/math/Timestep.h"
#include "engine/service/Service.h"

class ComponentUpdateService final : public Service
{
  public:
    ComponentUpdateService();

    // From Service
    void OnInit() override;
    void OnStart(ServiceProvider& service_provider) override;
    void OnUpdate() override;
    void OnCleanup() override;
    std::string_view GetName() const override;

  private:
    Timestep last_frame_;
};
