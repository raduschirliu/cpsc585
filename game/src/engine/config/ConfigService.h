#pragma once

#include "engine/service/Service.h"

class ConfigService final : public Service
{
    public:
        void OnInit() override;
        void OnStart(ServiceProvider& service_provider) override;
        void OnUpdate() override;
        void OnCleanup() override;
        std::string_view GetName() const override;
};