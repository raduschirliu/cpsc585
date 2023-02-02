#pragma once

#include <yaml-cpp/yaml.h>

#include <filesystem>

#include "VehicleData.h"
#include "engine/service/Service.h"

class ConfigService final : public Service
{
  public:
    ConfigService();
    void OnInit() override;
    void OnStart(ServiceProvider& service_provider) override;
    void OnUpdate() override;
    void OnCleanup() override;
    VehicleData GetVehicleData();
    std::string_view GetName() const override;

  private:
    std::vector<YAML::Node> config_files;
    std::filesystem::path path;
};