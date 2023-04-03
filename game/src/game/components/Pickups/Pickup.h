// this is the basic pickup class which will be inherited by the future children
// classes in order to implement the functionality and add meshes to them

#pragma once

#include <array>
#include <object_ptr.hpp>
#include <string>
#include <unordered_set>

#include "PickupType.h"
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/Transform.h"
#include "game/components/state/PlayerState.h"
#include "game/services/GameStateService.h"

class PlayerState;
class PickupService;

class Pickup : public Component, public IEventSubscriber<OnUpdateEvent>
{
  public:
    // From Component
    virtual void OnInit(const ServiceProvider& service_provider) override;
    virtual void OnStart();
    virtual void OnTriggerEnter(const OnTriggerEvent& data) override;
    virtual void OnTriggerExit(const OnTriggerEvent& data) override;
    virtual std::string_view GetName() const override;
    virtual void OnUpdate(const Timestep& delta_time) override;

  private:
    jss::object_ptr<PickupService> pickup_service_;

    bool powerup_executed_ = false;

  protected:
    jss::object_ptr<GameStateService> game_state_;
    // get the name of all ammo types and powerup types from pickup service
    std::array<std::string, 6> ammo_types_;
    std::array<std::string, 5> powerup_types_;

    jss::object_ptr<Transform> transform_;
    bool power_visible_ = true;

    // Get the names from game service, to check for which pickup picked the car
    // up.
    std::unordered_set<std::string> k_player_names_;

    PlayerState* player_state_ = nullptr;

    void SetPowerVisibility(bool bValue);
    void SetVehiclePowerup(PowerupPickupType type, const OnTriggerEvent& data);


    // for powerups
    float GetMaxRespawnTime(std::string type);
    float GetDeactivateTime(std::string type);
};
