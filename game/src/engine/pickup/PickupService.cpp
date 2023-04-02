#include "PickupService.h"

#include <array>
#include <assimp/Importer.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "engine/core/debug/Log.h"

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
}

std::string_view PickupService::GetName() const
{
    return "Powerup Service";
}

// From OnUpdateEvent
void PickupService::OnUpdate(const Timestep& delta_time)
{
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
            LoadPowerupDurationInformation(powerup_object, kPowerups[i]);
            LoadPowerupOtherInformation(powerup_object, kPowerups[i]);
        }
    }
}

void PickupService::LoadPowerupOtherInformation(
    const rapidjson::Value& powerup_object, const std::string& member)
{
    const rapidjson::Value& member_obj = powerup_object[member];
    if (member_obj.HasMember("max_speed"))
    {
        const rapidjson::Value& time = member_obj["max_speed"];

        // add to our damage map.
        powerup_max_speeds_.insert({member, time.GetFloat()});
    }
}

void PickupService::LoadPowerupDurationInformation(
    const rapidjson::Value& powerup_object, const std::string& member)
{
    const rapidjson::Value& member_obj = powerup_object[member];
    if (member_obj.HasMember("time"))
    {
        const rapidjson::Value& time = member_obj["time"];

        // add to our damage map.
        powerup_durations_.insert({member, time.GetFloat()});
    }
}

void PickupService::LoadAmmoInformation(const Document& doc)
{
    const rapidjson::Value& ammo_object = doc["Ammo"];
    for (int i = 0; i < kAmmoTypes.size(); i++)
    {
        if (ammo_object.HasMember(kAmmoTypes[i]))
        {
            LoadAmmoDamageInformation(ammo_object, kAmmoTypes[i]);
            LoadAmmoDurationInformation(ammo_object, kAmmoTypes[i]);
            LoadAmmoCooldownInformation(ammo_object, kAmmoTypes[i]);
            if (kAmmoTypes[i] == "Buckshot")
            {
                LoadAmmoOtherInformation(ammo_object, kAmmoTypes[i]);
            }
        }
    }
}

void PickupService::LoadAmmoOtherInformation(
    const rapidjson::Value& ammo_object, const std::string& member)
{
    const rapidjson::Value& member_object = ammo_object[member];
    if (member_object.HasMember("spread"))
    {
        const rapidjson::Value& spread = member_object["spread"];

        // add to our damage map.
        buckshot_additional_details_.insert({"spread", spread.GetFloat()});
    }
    if (member_object.HasMember("pellets"))
    {
        const rapidjson::Value& pellets = member_object["pellets"];

        // add to our damage map.
        buckshot_additional_details_.insert({"pellets", pellets.GetInt()});
    }
}

void PickupService::LoadAmmoCooldownInformation(
    const rapidjson::Value& ammo_object, const std::string& member)
{
    const rapidjson::Value& member_object = ammo_object[member];
    if (member_object.HasMember("cooldown"))
    {
        const rapidjson::Value& cooldown = member_object["cooldown"];

        // add to our damage map.
        ammo_cooldowns_.insert({member, cooldown.GetFloat()});
    }
}

void PickupService::LoadAmmoDurationInformation(
    const rapidjson::Value& ammo_object, const std::string& member)
{
    const rapidjson::Value& member_object = ammo_object[member];
    if (member_object.HasMember("time"))
    {
        const rapidjson::Value& time = member_object["time"];

        // add to our damage map.
        ammo_durations_.insert({member, time.GetFloat()});
    }
}

void PickupService::LoadAmmoDamageInformation(
    const rapidjson::Value& ammo_object, const std::string& member)
{
    const rapidjson::Value& member_object = ammo_object[member];
    if (member_object.HasMember("damage"))
    {
        const rapidjson::Value& damage = member_object["damage"];

        // add to our damage map.
        ammo_damages_.insert({member, damage.GetFloat()});
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
    if (ammo_damages_.find(ammo_type) != ammo_damages_.end())
    {
        return ammo_damages_[ammo_type];
    }
    return 0.0;
}

float PickupService::GetAmmoDuration(std::string ammo_type)
{
    if (ammo_durations_.find(ammo_type) != ammo_durations_.end())
    {
        return ammo_durations_[ammo_type];
    }
    return 0.0;
}

float PickupService::GetAmmoCooldown(std::string ammo_type)
{
    if (ammo_cooldowns_.find(ammo_type) != ammo_cooldowns_.end())
    {
        return ammo_cooldowns_[ammo_type];
    }
    return 0.0;
}

float PickupService::GetBuckshotAdditionalDetail(std::string detail_type)
{
    if (buckshot_additional_details_.find(detail_type) != ammo_damages_.end())
    {
        return ammo_damages_[detail_type];
    }
    return 0.0;
}

float PickupService::GetPowerupDuration(std::string powerup_type)
{
    if (powerup_durations_.find(powerup_type) != powerup_durations_.end())
    {
        return powerup_durations_[powerup_type];
    }
    return 0.0;
}

float PickupService::GetPowerupMaxSpeeds(std::string powerup_type)
{
    if (powerup_max_speeds_.find(powerup_type) != powerup_max_speeds_.end())
    {
        return powerup_max_speeds_[powerup_type];
    }
    return 0.0;
}