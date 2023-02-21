#include "engine/config/ConfigService.h"

#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <iostream>

#include "VehicleData.h"
#include "engine/core/debug/Log.h"
#include "engine/service/Service.h"

namespace fs = std::filesystem;

ConfigService::ConfigService() : config_files(), path("resources/config/")
{
}

void ConfigService::OnInit()
{
    // add all files in "config/" folder to a vector
    for (const fs::directory_entry& entry : fs::directory_iterator(path))
    {
        Log::info("[ConfigService] Found file: {}", entry.path().string());
        config_files.push_back(YAML::LoadFile(entry.path().string()));
    }
}

void ConfigService::OnStart(ServiceProvider& service_provider)
{
}

void ConfigService::OnUpdate()
{
}

void ConfigService::OnCleanup()
{
}

VehicleData ConfigService::GetVehicleData()
{
    VehicleData vehicleData;  // currently does literally NOTHING
    return vehicleData;
}

std::string_view ConfigService::GetName() const
{
    return "ConfigService";
}