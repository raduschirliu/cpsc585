#pragma once

#include "../service/Service.h"

class AIService final : public Service
{
  public:

    AIService();

    // From Service
    void OnInit() override;
    void OnStart(ServiceProvider& service_provider) override;
    void OnUpdate() override;
    void OnCleanup() override;
    std::string_view GetName() const override;
};


// Creating it here just for now
class NavMesh 
{

};