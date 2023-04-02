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

class PickupService final : public Service,
                             public IEventSubscriber<OnUpdateEvent>
{
  public:
    PickupService();

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

    /******** AMMO *******/
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

    /******** POWERUPS ******/
    void LoadPowerupInformation(const rapidjson::Document& doc);

    void LoadPowerupDurationInformation(const rapidjson::Value& powerup_object,
                                        const std::string& member);

    void LoadPowerupOtherInformation(const rapidjson::Value& powerup_object,
                                        const std::string& member);

    /****** GETTERS SETTERS *****/
    std::array<std::string, 6> GetAmmoPickupNames();
    float GetAmmoDamage(std::string ammo_type);
    float GetAmmoDuration(std::string ammo_type);
    float GetAmmoCooldown(std::string ammo_type);
    float GetBuckshotAdditionalDetail(std::string detail_type);

    std::array<std::string, 5> GetPowerupPickupNames();
    float GetPowerupDuration(std::string powerup_type);
    float GetPowerupMaxSpeeds(std::string powerup_type);

  private:
    /******** AMMO ********/
    std::unordered_map<std::string, float> ammo_damages_;
    std::unordered_map<std::string, float> ammo_durations_;
    std::unordered_map<std::string, float> ammo_cooldowns_;
    std::unordered_map<std::string, float> buckshot_additional_details_;

    /******** POWERUP ***/
    std::unordered_map<std::string, float> powerup_durations_;
    std::unordered_map<std::string, float> powerup_max_speeds_;
};