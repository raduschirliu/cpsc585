#pragma once

#include "engine/gui/OnGuiEvent.h"
#include "engine/service/Service.h"

class SceneDebugService final : public Service,
                                public IEventSubscriber<OnGuiEvent>
{
  public:
    // From Service
    void OnInit() override;
    void OnStart(ServiceProvider& service_provider) override;
    void OnUpdate() override;
    void OnCleanup() override;
    std::string_view GetName() const override;

    // From IEventSubscriber<OnGuiEvent>
    void OnGui() override;
};
