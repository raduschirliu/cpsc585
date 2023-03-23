#pragma once

#include <imgui.h>

#include <map>
#include <memory>
#include <object_ptr.hpp>
#include <set>
#include <unordered_map>
#include <vector>

#include "engine/core/Colors.h"
#include "engine/core/math/Timestep.h"
#include "engine/fwd/FwdServices.h"
#include "engine/gui/OnGuiEvent.h"
#include "engine/scene/Entity.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/service/Service.h"
#include "game/components/Pickups/PickupType.h"
#include "game/components/state/PlayerData.h"
#include "game/services/RaceConfig.h"

class Checkpoint;
class PlayerState;
class Transform;
class Texture;

enum class GameState : uint8_t
{
    kNotRunning = 0,
    kCountdown,
    kRaceInProgress,
    kPostRace
};

struct PlayerRecord
{
    uint32_t index;
    bool is_human;
    Entity* entity;
    Transform* transform;
    PlayerState* state_component;
    uint32_t checkpoint_count_accumulator;
    float progress_score;
};

struct GlobalRaceState
{
    GameState state;
    uint32_t finished_players;
    Timestep elapsed_time;
    Timestep countdown_elapsed_time;
    std::vector<PlayerRecord*> sorted_players;

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

    void RegisterCheckpoint(Entity& entity, Checkpoint* checkpoint);

    // setters
    void AddPlayerDetails(uint32_t id, PlayerStateData details);
    void AddPlayerStates(uint32_t id, PlayerState* states);
    void AddPlayerPowerup(uint32_t id, PowerupPickupType power);
    void RemovePlayerPowerup(uint32_t id);
    std::vector<std::pair<uint32_t, PowerupPickupType>> PowerupsActive();
    std::vector<std::pair<uint32_t, PowerupPickupType>> GetActivePowerups();
    void RemoveEveryoneSlowerSpeedMultiplier();
    void RemoveActivePowerup();

    // Events
    void PlayerCrossedCheckpoint(Entity& entity, uint32_t index);

    // Accessors
    void SetRaceConfig(const RaceConfig& config);
    const RaceConfig& GetRaceConfig() const;

    uint32_t GetEveryoneSlowerSpeedMultiplier();
    uint32_t GetDisableHandlingMultiplier();
    uint32_t GetHitBoxMultiplier();

    const GlobalRaceState& GetGlobalRaceState() const;
    const uint32_t GetNumCheckpoints() const;

    void PlayerFinished(Entity& entity);
    void PlayerCompletedLap(Entity& entity);

    GlobalRaceState& GetRaceState()
    {
        return race_state_;
    }

    double GetMaxCountdownSeconds();

  private:
    jss::object_ptr<AudioService> audio_service_;
    jss::object_ptr<GuiService> gui_service_;
    jss::object_ptr<AssetService> asset_service_;

    std::unordered_map<uint32_t, std::unique_ptr<PlayerRecord>> players_;

    GlobalRaceState race_state_;
    RaceConfig race_config_;
    TrackConfig track_config_;

    std::map<uint32_t, PlayerStateData> player_details_;
    std::map<uint32_t, PlayerState*> player_states_;
    std::map<uint32_t, PowerupPickupType> player_powers_;
    std::set<std::pair<uint32_t, PowerupPickupType>> same_powerup_;

    std::vector<std::pair<uint32_t, PowerupPickupType>> active_powerups_;
    std::map<std::pair<uint32_t, PowerupPickupType>, float> timer_;

    GameState stats_;

    void CheckTimer(double timer_limit, PowerupPickupType pickup_type);
    void UpdateRaceTimer(const Timestep& delta_time);
    void UpdatePlayerProgressScore(const Timestep& delta_time);

    void SetupRace();
    void StartRace();
    void PlayerCompletedLap(PlayerRecord& player);
    Entity& CreatePlayer(uint32_t index, bool is_human);
    CheckpointRecord& GetNextCheckpoint(uint32_t current_index);
    void StartCountdown();

    const Texture* countdown3_;
    const Texture* countdown2_;
    const Texture* countdown1_;

    ImFont* font_beya_;
    ImFont* font_pado_;
    ImFont* font_impact_;
};