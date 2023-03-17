#pragma once
#include <memory>

#include "PlayerData.h"
#include "engine/scene/Component.h"
#include "engine/scene/Entity.h"
#include "engine/scene/OnUpdateEvent.h"
#include "game/services/GameStateService.h"

class PlayerState : public Component, public IEventSubscriber<OnUpdateEvent>
{
  public:
    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    void OnStart() override;
    void OnUpdate(const Timestep& delta_time) override;
    std::string_view GetName() const override;

    // Setters
    void SetSpeedMultiplier(float value);
    void SetCurrentPowerup(PowerupPickupType type);
    void SetLapsCompleted(int laps);
    void SetLastCheckpoint(int checkpoint);
    void SetCurrentPlace(int place);

    // getters
    float GetSpeedMultiplier() const;
    int GetKills() const;
    int GetDeaths() const;
    int GetLapsCompleted() const;
    int GetLastCheckpoint() const;
    float GetHealth() const;
    Entity* GetNemesis();
    Entity* GetBullied();
    PowerupPickupType GetCurrentPowerup() const;
    int GetCurrentPlace() const;
    PlayerStateData* GetStateData();

  private:
    jss::object_ptr<GameStateService> game_state_service_;

    PlayerStateData player_state_;
};
