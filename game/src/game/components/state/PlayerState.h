#pragma once
#include <memory>

#include "PlayerData.h"
#include "engine/fwd/FwdServices.h"
#include "engine/render/MeshRenderer.h"
#include "engine/scene/Component.h"
#include "engine/scene/Entity.h"
#include "engine/scene/OnUpdateEvent.h"
#include "game/FwdGame.h"
#include "game/components/VehicleComponent.h"
#include "game/components/audio/AudioEmitter.h"

class PlayerState : public Component, public IEventSubscriber<OnUpdateEvent>
{
  public:
    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    void OnStart() override;
    void OnUpdate(const Timestep& delta_time) override;
    std::string_view GetName() const override;

    // setters
    void SetMaxCarSpeed(float max_speed);
    void SetHandlingMultiplier(float multiplier);
    void SetHealth(float health);
    void DecrementHealth(float health);
    void IncrementHealth(float health);
    void SetSpeedMultiplier(float value);
    void SetCurrentPowerup(PowerupPickupType type);
    void SetCurrentAmmoType(AmmoPickupType type);
    void SetLapsCompleted(int laps);
    void SetLastCheckpoint(int checkpoint);
    void SetCurrentPlace(int place);
    void SetPlayerWhoShotMe(std::string player);
    void SetPlayerName(std::string name);
    void SetIsAccelerating(bool b_value);
    void SetCurrentSpeed(float value);

    // getters
    float GetMaxCarSpeed();
    float GetHandlingMultiplier();
    bool IsDead() const;
    float GetDeathCooldown() const;
    float GetHealth() const;
    float GetSpeedMultiplier() const;
    int GetKills() const;
    int GetDeaths() const;
    int GetLapsCompleted() const;
    int GetLastCheckpoint() const;
    float GetCurrentSpeed() const;
    Entity* GetNemesis();
    Entity* GetBullied();
    PowerupPickupType GetCurrentPowerup() const;
    AmmoPickupType GetCurrentAmmoType() const;
    int GetCurrentPlace() const;
    PlayerStateData* GetStateData();
    std::string GetPlayerWhoShotMe();
    std::string GetPlayerName();
    bool GetIsAccelerating();

  private:
    void CheckDead(const Timestep& delta_time);

    PlayerStateData player_state_;
    float death_cooldown_;  /// time until a dead player revives

    /* ----- service and component dependencies ----- */

    jss::object_ptr<PhysicsService> physics_service_;
    jss::object_ptr<GameStateService> game_state_service_;
    jss::object_ptr<AssetService> asset_service_;
    jss::object_ptr<RenderService> render_service_;

    jss::object_ptr<Transform> transform_;
    jss::object_ptr<VehicleComponent> vehicle_;
    jss::object_ptr<AudioEmitter> audio_emitter_;
    jss::object_ptr<MeshRenderer> renderer_;
    jss::object_ptr<ParticleSystem> explosion_particles_;
};
