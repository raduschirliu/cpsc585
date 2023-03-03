// this is the basic pickup class which will be inherited by the future children
// classes in order to implement the functionality and add meshes to them

#pragma once

#include <object_ptr.hpp>

#include "engine/game_state/GameStateService.h"
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/Transform.h"
#include "game/components/state/PlayerState.h"
#include "PickupType.h"

class PlayerState;


class Pickup : public Component, public IEventSubscriber<OnUpdateEvent>
{
  public:
    // From Component
    virtual void OnInit(const ServiceProvider& service_provider) override;
    virtual void OnTriggerEnter(const OnTriggerEvent& data) override;
    virtual void OnTriggerExit(const OnTriggerEvent& data) override;
    virtual std::string_view GetName() const override;
    virtual void OnUpdate(const Timestep& delta_time) override;

  private:
    jss::object_ptr<Transform> transform_;
    jss::object_ptr<GameStateService> game_state_;

    bool powerup_executed_ = false;

  protected:
    bool power_visible_ = true;

    PlayerState* player_state_ = nullptr;

    void SetPowerVisibility(bool bValue);
};
