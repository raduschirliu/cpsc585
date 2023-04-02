#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "engine/fwd/FwdServices.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/service/Service.h"
#include "game/components/Pickups/PickupType.h"

class PowerupService final : public Service,
                             public IEventSubscriber<OnUpdateEvent>
{
  public:
    PowerupService();

    // From Service
    void OnInit() override;
    void OnUpdate() override;
    void OnCleanup() override;
    void OnStart(ServiceProvider& service_provider);
    void OnSceneLoaded(Scene& scene) override;
    std::string_view GetName() const override;

    // From OnUpdateEvent
    void OnUpdate(const Timestep& delta_time);

    // load the values from json file
    void LoadAssetFile(const std::string& path);

  private:
};