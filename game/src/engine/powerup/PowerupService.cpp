#include "PowerupService.h"

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

#include <assimp/Importer.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "engine/asset/AssetBundle.h"
#include "engine/core/debug/Log.h"

using rapidjson::Document;
using rapidjson::IStreamWrapper;
using rapidjson::SizeType;
using std::string;

static const string kPowerupFilePath = "resources/powerup/PowerupDetails.jsonc";

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
        const rapidjson::Value& ammo_object = doc["Ammo"];
        if(ammo_object.HasMember("Buckshot"))
        {
            const rapidjson::Value& buckshot_object = ammo_object["buckshot"];
            
        }
    }

    if (doc.HasParseError())
    {
        return;
    }
}