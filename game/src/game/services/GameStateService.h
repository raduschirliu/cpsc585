#pragma once

#include <imgui.h>

#include <chrono>
#include <map>
#include <memory>
#include <object_ptr.hpp>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
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
    double finished_time;
};

struct GlobalRaceState
{
    GameState state;
    uint32_t finished_players;
    uint32_t total_players;
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

    // Events
    void PlayerCrossedCheckpoint(Entity& entity, uint32_t index);

    // Accessors
    void SetRaceConfig(const RaceConfig& config);
    const RaceConfig& GetRaceConfig() const;

    const GlobalRaceState& GetGlobalRaceState() const;
    const uint32_t GetNumCheckpoints() const;

    GlobalRaceState& GetRaceState()
    {
        return race_state_;
    }

    double GetMaxCountdownSeconds();
    int GetCurrentCheckpoint(uint32_t entity_id,
                             glm::vec3& out_checkpoint_location,
                             glm::vec3& out_checkpoint_location2);
    void SetRespawnEntity(uint32_t entity_id);

    // getter for respawn
    bool GetRespawnRequested(uint32_t entity_id);

    // setter for respawn
    void AddRespawnPlayers(uint32_t entity_id);
    void RemoveRespawnPlayers(uint32_t entity_id);

    // Powerups

    std::unordered_set<std::string> GetPlayerStaticNames();

  protected:
    void KillFeed(const ImGuiWindowFlags& flags);
    void DisplayKillFeed();
    double kill_feed_timer_ = 0.0f;
    bool display_kill_details_ = false;

  private:
    jss::object_ptr<AudioService> audio_service_;
    jss::object_ptr<GuiService> gui_service_;
    jss::object_ptr<AssetService> asset_service_;
    jss::object_ptr<SceneDebugService> scene_service_;
    jss::object_ptr<InputService> input_service_;
    jss::object_ptr<PhysicsService> physics_service_;

    std::unordered_map<uint32_t, std::unique_ptr<PlayerRecord>> players_;

    GlobalRaceState race_state_;
    RaceConfig race_config_;
    TrackConfig track_config_;

    std::map<uint32_t, PlayerStateData> player_details_;
    std::map<uint32_t, PlayerState*> player_states_;
    std::map<uint32_t, PowerupPickupType> player_powers_;
    std::set<std::pair<uint32_t, PowerupPickupType>> same_powerup_;

    // players requesting to respawn will be handled by this.
    std::unordered_set<uint32_t> players_respawn_;

    std::vector<std::pair<uint32_t, PowerupPickupType>> active_powerups_;
    std::map<std::pair<uint32_t, PowerupPickupType>, float> timer_;

    // To store the powerup's information along with where it should be spawned
    // in the map.
    std::vector<std::pair<PowerupPickupType, glm::vec3>> powerup_info;
    std::vector<std::pair<AmmoPickupType, glm::vec3>> ammo_info_;

    std::unordered_map<std::string,
                       std::chrono::time_point<std::chrono::system_clock>>
        timestamp_map;
    std::unordered_set<std::string> kill_feed_info_;

    GameState stats_;

    void UpdateRaceTimer(const Timestep& delta_time);
    void UpdatePlayerProgressScore(const Timestep& delta_time);
    void UpdatePowerupInfo();

    void SetupRace();
    void StartRace();
    void SetupPowerups();
    void PlayerCompletedLap(PlayerRecord& player);
    Entity& CreatePlayer(uint32_t index, bool is_human);
    CheckpointRecord& GetNextCheckpoint(uint32_t current_index);
    void StartCountdown();
    void DisplayScoreboard();

    bool display_pause_ = false;

    const Texture* countdown3_;
    const Texture* countdown2_;
    const Texture* countdown1_;
    const Texture* home_button_;
    const Texture* ending_;
    const Texture* record_;
    const Texture* disableHandling_;
    const Texture* everyoneSlower_;
    const Texture* increaseAimBox_;
    const Texture* killAbilities_;
    const Texture* pause_;

    ImFont* font_beya_;
    ImFont* font_pado_;
    ImFont* font_impact_;
    ImFont* font_cookie_;
    ImFont* font_koverwatch_;
    ImFont* font_mandu_;
    ImFont* font_pixel_;

    std::pair<std::string, int> most_kills = {"", -1};
    std::pair<std::string, int> least_deaths = {"", 1000};
};