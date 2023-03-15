#pragma once

#include <map>
#include <memory>
#include <object_ptr.hpp>
#include <set>
#include <utility>
#include <vector>

#include "engine/audio/AudioService.h"
#include "engine/core/math/Timestep.h"
#include "engine/gui/OnGuiEvent.h"
#include "engine/scene/Entity.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/service/Service.h"
#include "game/components/Pickups/PickupType.h"
#include "game/components/state/PlayerStateStruct.h"

class PlayerState;

enum class GameState : uint8_t
{
    kNotRunning = 0,
    kCountdown,
    kRunning,
    kFinished
};

struct GameStats
{
    GameState state;
    size_t num_players;
    size_t finished_players;
    size_t num_laps;
    Timestep elapsed_time;
    Timestep countdown_elapsed_time;

    void Reset();
};

class GameStateService : public Service, public IEventSubscriber<OnGuiEvent>
{
  public:
    GameStateService();

    // From Service
    void OnInit() override;
    void OnUpdate() override;
    void OnCleanup() override;
    void OnStart(ServiceProvider& service_provider);
    void OnSceneLoaded(Scene& scene) override;
    std::string_view GetName() const override;

    // From Event subscribers
    void OnGui() override;

  private:
    jss::object_ptr<AudioService> audio_service_;

    std::map<uint32_t, PlayerStateData> player_details_;
    std::map<uint32_t, PlayerState*> player_states_;
    std::map<uint32_t, PowerupPickupType> player_powers_;
    std::set<std::pair<uint32_t, PowerupPickupType>> same_powerup_;

    std::vector<std::pair<uint32_t, PowerupPickupType>> active_powerups_;
    std::map<std::pair<uint32_t, PowerupPickupType>, float> timer_;

    GameStats stats_;

    void StartCountdown();
    void StartGame();

  public:
    // setters
    void AddPlayerDetails(uint32_t id, PlayerStateData details);
    void AddPlayerStates(uint32_t id, PlayerState* states);
    void AddPlayerPowerup(uint32_t id, PowerupPickupType power);
    void RemovePlayerPowerup(uint32_t id);

    std::vector<std::pair<uint32_t, PowerupPickupType>> PowerupsActive();

    std::vector<std::pair<uint32_t, PowerupPickupType>> GetActivePowerups();

    uint32_t GetEveryoneSlowerSpeedMultiplier();
    uint32_t GetDisableHandlingMultiplier();
    uint32_t GetHitBoxMultiplier();

    void RemoveEveryoneSlowerSpeedMultiplier();
    void RemoveActivePowerup();

    void PlayerFinished(Entity& entity);
    void PlayerCompletedLap(Entity& entity);
};