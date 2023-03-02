// this is the basic pickup class which will be inherited by the future children
// classes in order to implement the functionality and add meshes to them

#pragma once

#include <object_ptr.hpp>

#include "engine/scene/Component.h"
#include "engine/scene/Transform.h"

enum class PowerupPickupType
{
    kDefaultPowerup = 0,
    kDisableHandling,
    kEveryoneSlower,
    kIncreaseAimBox,
    kKillAbilities
};

enum class AmmoPickupType
{
    kDefaultAmmo = 0,
    kBuckshot,
    kDoubleDamage,
    kExploadingBullet,
    kIncreaseFireRate,
    kVampireBullet
};

class Pickup : public Component
{
  public:
    // From Component
    virtual void OnInit(const ServiceProvider& service_provider) override;
    virtual void OnTriggerEnter(const OnTriggerEvent& data) override;
    virtual void OnTriggerExit(const OnTriggerEvent& data) override;
    virtual std::string_view GetName() const override;

  private:
    jss::object_ptr<Transform> transform_;

    void SetPowerActivated(bool bValue);

  protected:
    bool power_activated_ = true;
};
