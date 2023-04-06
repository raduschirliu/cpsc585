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

struct AmmoInformation
{
    float damages;
    float respawn;
    float time;
    int pellets;
    int spread;
    float cooldown;

    AmmoInformation()
    {
        damages = 0.f;
        cooldown = 0.f;
        time = 0.f;
        respawn = 0.f;
        pellets = 0;
        spread = 0;
    }
};

struct PowerupInformation
{
    float max_speed;
    float respawn;
    float time;

    PowerupInformation()
    {
        time = 0.f;
        respawn = 0.f;
        max_speed = 0.f;
    }
};

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

    /******** POWERUPS ******/
    void LoadPowerupInformation(const rapidjson::Document& doc);

    /****** GETTERS SETTERS *****/
    std::array<std::string, 6> GetAmmoPickupNames();
    float GetAmmoDamage(std::string ammo_type);
    float GetAmmoDuration(std::string ammo_type);
    float GetAmmoCooldown(std::string ammo_type);
    float GetAmmoRespawnTime(std::string ammo_type);
    float GetAmmoSpread(std::string ammo_type);
    float GetAmmoPellets(std::string ammo_type);

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
    std::unordered_map<std::string, AmmoInformation> ammo_information_;

    /******** POWERUP ***/
    std::unordered_map<std::string, PowerupInformation> powerup_information_;

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