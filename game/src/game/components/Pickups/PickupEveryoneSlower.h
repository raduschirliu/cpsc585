// this is the basic pickup class which will be inherited by the future children
// classes in order to implement the functionality and add meshes to them

#pragma once

#include <object_ptr.hpp>

#include "engine/scene/Component.h"
#include "engine/scene/Transform.h"

class PickupEveryoneSlower : public Component
{
  public:
    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    void OnTriggerEnter(const OnTriggerEvent& data) override;
    void OnTriggerExit(const OnTriggerEvent& data) override;
    std::string_view GetName() const override;

  private:
    jss::object_ptr<Transform> transform_;

    bool power_activated_ = true;

    inline void SetPowerActivated(bool bValue)
    {
        power_activated_ = false;
    }
};
