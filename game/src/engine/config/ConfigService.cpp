#include <filesystem>
#include <iostream>

#include <yaml-cpp/yaml.h>

#include "engine/config/ConfigService.h"
#include "engine/core/debug/Log.h"
#include "engine/service/Service.h"

namespace fs = std::filesystem;

void ConfigService::OnInit()
{
    Log::info("[ConfigService] Initializing");
    
    std::vector<YAML::Node> config_files(0);
    fs::path path ="resources/config/";
    
    for (const fs::directory_entry& entry : fs::directory_iterator(path))
    {
        Log::debug("[ConfigService] Found file: {}", entry.path().string());
        config_files.push_back(YAML::LoadFile(entry.path().string()));
    }

    // parse testing
    // for (YAML::Node file : config_files) {
    //     YAML::Node number = file["number"];
    //     YAML::Node numbers = file["numbers"];
    //     YAML::Node string = file["string"];
    //     YAML::Node strings = file["strings"];
    // }
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

std::string_view ConfigService::GetName() const
{
    return "ConfigService";
}