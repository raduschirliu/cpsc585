#pragma once
#include <memory>

#include "PlayerStateStruct.h"
#include "engine/game_state/GameStateService.h"
#include "engine/scene/Component.h"
#include "engine/scene/Entity.h"
#include "engine/scene/OnUpdateEvent.h"

class PlayerState : public Component, public IEventSubscriber<OnUpdateEvent>
{
  public:
    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    void OnStart() override;
    void OnUpdate(const Timestep& delta_time) override;
    std::string_view GetName() const override;

  private:
    PlayerStateData player_state_;
    jss::object_ptr<GameStateService> game_state_service_;

    bool game_state_assigned_ = false;

  public:
    // getters
    float GetSpeedMultiplier();

    int GetKills();

    int GetDeaths();

    int GetLapsCompleted();

    int GetCurrentLap();

    Entity* GetNemesis();

    Entity* GetBullied();

    double GetTimeElapsed();

    int GetCurrentPowerup();

    PlayerStateData* GetStateData();
};
