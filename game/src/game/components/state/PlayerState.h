#pragma once
#include <memory>

#include "PlayerStateStruct.h"
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/game_state/GameStateService.h"
#include "engine/scene/Entity.h"

class PlayerState : public Component, public IEventSubscriber<OnUpdateEvent>
{
  public:
    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    void OnStart() override;
    void OnUpdate(const Timestep& delta_time) override;
    std::string_view GetName() const override;

  private:
    PlayerStateStruct player_state_;
    jss::object_ptr<GameStateService> game_state_service_;

    Entity* attached_entity_;

    bool game_state_assigned_ = false;

  public:
    // getters
    inline std::shared_ptr<double> GetSpeed()
    {
        return player_state_.speed;
    }

    inline int GetKills()
    {
        return player_state_.number_kills;
    }

    inline int GetDeaths()
    {
        return player_state_.number_deaths;
    }

    inline int GetLapsCompleted()
    {
        return player_state_.laps_completed;
    }

    inline int GetCurrentLap()
    {
        return player_state_.current_lap;
    }

    inline Entity* GetNemesis()
    {
        return player_state_.nemesis;
    }

    inline Entity* GetBullied()
    {
        return player_state_.bullied;
    }

    inline double GetTimeElapsed()
    {
        return player_state_.time_elapsed;
    }

    inline int GetCurrentPowerup()
    {
        return player_state_.current_powerup;
    }

    // setters
    void SetSpeed(std::shared_ptr<double> speed)
    {
        if(speed)
            player_state_.speed = speed;
    }

    void SetEntity(Entity& entity)
    {
        attached_entity_ = &entity;
    }
};