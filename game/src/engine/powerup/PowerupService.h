#pragma once

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

#include <array>
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

    void LoadAmmoInformation(const rapidjson::Document& doc);
    void LoadAmmoDamageInformation(const rapidjson::Value& ammo_object,
                                   const std::string& member);
    void LoadAmmoDurationInformation(const rapidjson::Value& ammo_object,
                                     const std::string& member);
    void LoadAmmoCooldownInformation(const rapidjson::Value& ammo_object,
                                     const std::string& member);

    // Only for buckshot as it has other information as well. 
    void LoadAmmoOtherInformation(const rapidjson::Value& ammo_object,
                                     const std::string& member);

    void LoadPickupInformation(const rapidjson::Document& doc);

  private:
    std::unordered_map<std::string, float> ammo_damages_;
    std::unordered_map<std::string, float> ammo_durations_;
    std::unordered_map<std::string, float> ammo_cooldowns_;
    std::unordered_map<std::string, float> buckshot_additional_details_;
};  