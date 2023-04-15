#include "PickupService.h"

#include <engine/scene/Entity.h>

#include <array>
#include <assimp/Importer.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "engine/core/debug/Log.h"
#include "game/components/state/PlayerState.h"

using rapidjson::Document;
using rapidjson::IStreamWrapper;
using rapidjson::SizeType;
using std::string;

static const string kPowerupFilePath = "resources/powerup/PowerupDetails.jsonc";

// matching the names in json file so that we can easily loop
static const std::array<string, 6> kAmmoTypes = {
    "Default",    "Buckshot",         "DoubleDamage",
    "Exploading", "IncreaseFireRate", "Vampire"};

static const std::array<string, 5> kPowerups = {
    "Default", "DisableHandling", "EveryoneSlower", "IncreaseAimBox",
    "KillAbilities"};

PickupService::PickupService()
{
}

// From Service
void PickupService::OnInit()
{
    LoadAssetFile(kPowerupFilePath);
    GetEventBus().Subscribe<OnUpdateEvent>(this);
}

void PickupService::OnStart(ServiceProvider& service_provider)
{
}

void PickupService::OnUpdate()
{
}

void PickupService::OnCleanup()
{
}

void PickupService::OnSceneLoaded(Scene& scene)
{
    active_powerup_ = "";
}

std::string_view PickupService::GetName() const
{
    return "Powerup Service";
}

// From OnUpdateEvent
void PickupService::OnUpdate(const Timestep& delta_time)
{
    for (auto& time : entity_timer_powerups_)
    {
        time.second += delta_time.GetSeconds();
    }
    HandleDisablingPowerup();
}

void PickupService::HandleDisablingPowerup()
{
    // loop through the timers to see if any one of those have exceeded the max
    // time.
    for (const auto& time : entity_timer_powerups_)
    {
        if (entity_holding_powerups_[time.first] == "DisableHandling")
        {
            active_powerup_ = "DisableHandling";
        }
        else if (entity_holding_powerups_[time.first] == "EveryoneSlower")
        {
            active_powerup_ = "EveryoneSlower";
        }
        else if (entity_holding_powerups_[time.first] == "IncreaseAimBox")
        {
            active_powerup_ = "IncreaseAimBox";
        }
        else if (entity_holding_powerups_[time.first] == "KillAbilities")
        {
            active_powerup_ = "KillAbilities";
        }

        if (!time.first)
        {
            continue;
        }

        auto& entity = time.first;

        if (!entity)
        {
            continue;
        }

        if (time.second >
            powerup_information_[entity_holding_powerups_[time.first]].time)
        {
            // if the powerup was disable handling
            if (entity_holding_powerups_[time.first] == "DisableHandling")
            {
                // set the powerup to default for this player
                if (entity->HasComponent<PlayerState>())
                {
                    auto& player_state = entity->GetComponent<PlayerState>();
                    player_state.SetCurrentPowerup(
                        PowerupPickupType::kDefaultPowerup);

                    auto& audio_emitter = entity->GetComponent<AudioEmitter>();
                    audio_emitter.PlaySource("pickup_get_02.ogg");
                }
                not_disabled_entities_.erase(entity);

                entity_holding_powerups_.erase(time.first);
                entity_timer_powerups_.erase(time.first);
                break;
            }

            // if the powerup was speed multiplier
            else if (entity_holding_powerups_[time.first] == "EveryoneSlower")
            {
                // set the powerup to default for this player
                if (entity->HasComponent<PlayerState>())
                {
                    auto& player_state = entity->GetComponent<PlayerState>();
                    player_state.SetCurrentPowerup(
                        PowerupPickupType::kDefaultPowerup);

                    auto& audio_emitter = entity->GetComponent<AudioEmitter>();
                    audio_emitter.PlaySource("pickup_get_02.ogg");
                }
                not_slow_entities_.erase(entity);

                entity_holding_powerups_.erase(time.first);
                entity_timer_powerups_.erase(time.first);
                break;
            }

            else if (entity_holding_powerups_[time.first] == "KillAbilities")
            {
                // set the powerup to default for this player
                if (entity->HasComponent<PlayerState>())
                {
                    auto& player_state = entity->GetComponent<PlayerState>();
                    player_state.SetCurrentPowerup(
                        PowerupPickupType::kDefaultPowerup);

                    auto& audio_emitter = entity->GetComponent<AudioEmitter>();
                    audio_emitter.PlaySource("pickup_get_02.ogg");
                }

                not_slow_entities_.clear();
                not_disabled_entities_.clear();

                entity_holding_powerups_.clear();
                entity_timer_powerups_.clear();
                break;
            }
        }
    }
}

void PickupService::LoadAssetFile(const string& path)
{
    std::ifstream file_stream(path);
    ASSERT_MSG(file_stream.is_open(), "Failed to open asset file");

    IStreamWrapper stream(file_stream);
    Document doc;

    string jsonStr;
    jsonStr.assign((std::istreambuf_iterator<char>(file_stream)),
                   (std::istreambuf_iterator<char>()));

    doc.Parse(jsonStr.c_str());

    if (doc.HasMember("Ammo"))
    {
        LoadAmmoInformation(doc);
    }

    if (doc.HasMember("Powerup"))
    {
        LoadPowerupInformation(doc);
    }

    if (doc.HasParseError())
    {
        return;
    }
}

void PickupService::LoadPowerupInformation(const rapidjson::Document& doc)
{
    const rapidjson::Value& powerup_object = doc["Powerup"];
    for (int i = 0; i < kPowerups.size(); i++)
    {
        if (powerup_object.HasMember(kPowerups[i]))
        {
            const rapidjson::Value& member_object =
                powerup_object[kPowerups[i]];
            PowerupInformation info;
            if (member_object.HasMember("max_speed"))
            {
                info.max_speed = member_object["max_speed"].GetFloat();
            }
            if (member_object.HasMember("respawn"))
            {
                info.respawn = member_object["respawn"].GetFloat();
            }
            if (member_object.HasMember("time"))
            {
                info.time = member_object["time"].GetFloat();
            }
            powerup_information_.insert({kPowerups[i], info});
        }
    }
}

void PickupService::LoadAmmoInformation(const Document& doc)
{
    const rapidjson::Value& ammo_object = doc["Ammo"];
    for (int i = 0; i < kAmmoTypes.size(); i++)
    {
        if (ammo_object.HasMember(kAmmoTypes[i]))
        {
            const rapidjson::Value& member_object = ammo_object[kAmmoTypes[i]];
            AmmoInformation info;
            if (member_object.HasMember("damage"))
            {
                info.damages = member_object["damage"].GetFloat();
            }
            if (member_object.HasMember("respawn"))
            {
                info.respawn = member_object["respawn"].GetFloat();
            }
            if (member_object.HasMember("time"))
            {
                info.time = member_object["time"].GetFloat();
            }
            if (member_object.HasMember("pellets"))
            {
                info.pellets = member_object["pellets"].GetInt();
            }
            if (member_object.HasMember("cooldown"))
            {
                info.cooldown = member_object["cooldown"].GetFloat();
            }
            if (member_object.HasMember("spread"))
            {
                info.spread = member_object["spread"].GetInt();
            }

            ammo_information_.insert({kAmmoTypes[i], info});
        }
    }
}

/****** GETTERS SETTERS *****/

std::array<std::string, 6> PickupService::GetAmmoPickupNames()
{
    return kAmmoTypes;
}

std::array<std::string, 5> PickupService::GetPowerupPickupNames()
{
    return kPowerups;
}

float PickupService::GetAmmoDamage(std::string ammo_type)
{
    if (ammo_information_.find(ammo_type) != ammo_information_.end())
    {
        return ammo_information_[ammo_type].damages;
    }
    return 0.0;
}

float PickupService::GetAmmoDuration(std::string ammo_type)
{
    if (ammo_information_.find(ammo_type) != ammo_information_.end())
    {
        return ammo_information_[ammo_type].time;
    }
    return 0.0;
}

float PickupService::GetAmmoCooldown(std::string ammo_type)
{
    if (ammo_information_.find(ammo_type) != ammo_information_.end())
    {
        return ammo_information_[ammo_type].cooldown;
    }
    return 0.0;
}

float PickupService::GetAmmoSpread(std::string ammo_type)
{
    if (ammo_information_.find(ammo_type) != ammo_information_.end())
    {
        return ammo_information_[ammo_type].spread;
    }
    return 0.0;
}

float PickupService::GetAmmoPellets(std::string ammo_type)
{
    if (ammo_information_.find(ammo_type) != ammo_information_.end())
    {
        return ammo_information_[ammo_type].pellets;
    }
    return 0.0;
}

float PickupService::GetAmmoRespawnTime(std::string ammo_type)
{
    if (ammo_information_.find(ammo_type) != ammo_information_.end())
    {
        return ammo_information_[ammo_type].respawn;
    }
    return 0.0;
}

float PickupService::GetPowerupDuration(std::string powerup_type)
{
    if (powerup_information_.find(powerup_type) != powerup_information_.end())
    {
        return powerup_information_[powerup_type].time;
    }
    return 0.0;
}

float PickupService::GetPowerupMaxSpeeds(std::string powerup_type)
{
    if (powerup_information_.find(powerup_type) != powerup_information_.end())
    {
        return powerup_information_[powerup_type].max_speed;
    }
    return 0.0;
}

float PickupService::GetPowerupRespawnTime(std::string powerup_type)
{
    if (powerup_information_.find(powerup_type) != powerup_information_.end())
    {
        return powerup_information_[powerup_type].respawn;
    }
    return 0.0;
}

float PickupService::GetEntityPowerupTimer(Entity* entity)
{
    return entity_timer_powerups_[entity];
}

void PickupService::AddEntityWithPowerup(Entity* entity,
                                         const std::string& powerup)
{
    entity_holding_powerups_.insert({entity, powerup});

    if (powerup == "DisableHandling")
    {
        if (entity)
        {
            not_disabled_entities_.insert(entity);
        }
    }

    else if (powerup == "EveryoneSlower")
    {
        if (entity)
        {
            not_slow_entities_.insert(entity);
        }
    }
}

void PickupService::AddEntityWithTimer(Entity* entity, const float& timer)
{
    entity_timer_powerups_.insert({entity, timer});
}

bool PickupService::IsVehicleSlowDown(Entity* entity)
{
    if (not_slow_entities_.find(entity) != not_slow_entities_.end() ||
        not_slow_entities_.empty())
    {
        // as this was the vehicle which executed the powerup.
        return false;
    }
    // as this vehicle did not execute the powerup.
    return true;
}

bool PickupService::IsVehicleDisableHandling(Entity* entity)
{
    if (not_disabled_entities_.find(entity) != not_disabled_entities_.end() ||
        not_disabled_entities_.empty())
    {
        // as this was the vehicle which executed the powerup.
        return false;
    }
    // as this vehicle did not execute the powerup.
    return true;
}

std::string PickupService::GetActivePowerup()
{
    return active_powerup_;
}

void PickupService::SetActivePowerup(std::string powerup)
{
    active_powerup_ = powerup;
}