#include "PowerupService.h"

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

PowerupService::PowerupService()
{
}

// From Service
void PowerupService::OnInit()
{
    LoadAssetFile(kPowerupFilePath);
}

void PowerupService::OnStart(ServiceProvider& service_provider)
{
}

void PowerupService::OnUpdate()
{
}

void PowerupService::OnCleanup()
{
}

void PowerupService::OnSceneLoaded(Scene& scene)
{
}

std::string_view PowerupService::GetName() const
{
    return "Powerup Service";
}

// From OnUpdateEvent
void PowerupService::OnUpdate(const Timestep& delta_time)
{
}

void PowerupService::LoadAssetFile(const string& path)
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

void PowerupService::LoadPowerupInformation(const rapidjson::Document& doc)
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

void PowerupService::LoadPowerupOtherInformation(
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

void PowerupService::LoadPowerupDurationInformation(
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

void PowerupService::LoadAmmoInformation(const Document& doc)
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

void PowerupService::LoadAmmoOtherInformation(
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

void PowerupService::LoadAmmoCooldownInformation(
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

void PowerupService::LoadAmmoDurationInformation(
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

void PowerupService::LoadAmmoDamageInformation(
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
