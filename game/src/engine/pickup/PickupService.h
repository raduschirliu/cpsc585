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
    void LoadAmmoRespawnTimeInformation(const rapidjson::Value& ammo_object,
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
    void LoadPowerupRespawnTimeInformation(
        const rapidjson::Value& powerup_object, const std::string& member);

    /****** GETTERS SETTERS *****/
    std::array<std::string, 6> GetAmmoPickupNames();
    float GetAmmoDamage(std::string ammo_type);
    float GetAmmoDuration(std::string ammo_type);
    float GetAmmoCooldown(std::string ammo_type);
    float GetAmmoRespawnTime(std::string ammo_type);
    float GetBuckshotAdditionalDetail(std::string detail_type);

    std::array<std::string, 5> GetPowerupPickupNames();
    float GetPowerupDuration(std::string powerup_type);
    float GetPowerupMaxSpeeds(std::string powerup_type);
    float GetPowerupRespawnTime(std::string powerup_type);

    /********* EXECUTING AND HANDLING THE POWERUPS ************/

    // Handle disabling the powerup
    void HandleDisablingPowerup();

    // Add the entity with the powerup they are holding
    void AddEntityWithPowerup(Entity* entity, const std::string& powerup);

    // Add the timers for the powerup when they are executed.
    void AddEntityWithTimer(Entity* entity, const float& timer);

    // To tell if the car should have the slowing down multiplier to them or not
    bool IsVehicleSlowDown(Entity* entity);

    // To tell if the car should have the slowing down multiplier to them or not
    bool IsVehicleDisableHandling(Entity* entity);

  private:
    /******** AMMO ********/
    std::unordered_map<std::string, float> ammo_damages_;
    std::unordered_map<std::string, float> ammo_durations_;
    std::unordered_map<std::string, float> ammo_cooldowns_;
    std::unordered_map<std::string, float> buckshot_additional_details_;
    std::unordered_map<std::string, float> ammo_respawn_times_;

    /******** POWERUP ***/
    std::unordered_map<std::string, float> powerup_durations_;
    std::unordered_map<std::string, float> powerup_max_speeds_;
    std::unordered_map<std::string, float> powerup_respawn_times_;

    /********* EXECUTING AND HANDLING THE POWERUPS ************/

    // Storing the {entity, powerup}
    std::unordered_map<Entity*, std::string> entity_holding_powerups_;

    // Storing the {entity, timer}
    std::unordered_map<Entity*, float> entity_timer_powerups_;

    // Disable Handling for those players who are not in this map
    std::unordered_set<Entity*> not_disabled_entities_;

    // Slower the speed of those entities which are not in this list.
    std::unordered_set<Entity*> not_slow_entities_;
};