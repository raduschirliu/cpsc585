#include "GameStateService.h"

#include <imgui.h>

#include <array>
#include <string>

#include "engine/App.h"
#include "engine/core/debug/Log.h"
#include "engine/physics/Hitbox.h"
#include "engine/render/Camera.h"
#include "engine/render/MeshRenderer.h"
#include "engine/scene/OnUpdateEvent.h"
#include "game/components/Controllers/AIController.h"
#include "game/components/Controllers/PlayerController.h"
#include "game/components/FollowCamera.h"
#include "game/components/PlayerHud.h"
#include "game/components/RaycastComponent.h"
#include "game/components/VehicleComponent.h"
#include "game/components/state/PlayerState.h"

using glm::vec3;
using std::array;
using std::string;

// so that as soon as 5 seconds are hit the
// powerup is disabled and removed.
static constexpr float kSlowDownTimerLimit = 5.0f;
static constexpr uint32_t kMaxPlayers = 4;

static const Timestep kCountdownTime = Timestep::Seconds(12.0);
static const Timestep kMinRaceTime = Timestep::Seconds(50.0);

static const array<string, kMaxPlayers> kHumanPlayerNames = {
    "Player 1", "Player 2", "Player 3", "Player 4"};
static const array<string, kMaxPlayers> kAiPlayerNames = {"CPU 1", "CPU 2",
                                                          "CPU 3", "CPU 4"};

void GlobalRaceState::Reset()
{
    state = GameState::kNotRunning;
    countdown_elapsed_time.SetSeconds(0);
    elapsed_time.SetSeconds(0);
    finished_players = 0;
}

GameStateService::GameStateService()
{
}

void GameStateService::OnInit()
{
    race_config_.num_human_players = 1;
    race_config_.num_ai_players = 3;
    race_config_.num_laps = 2;

    track_config_.player_spawns = {
        {.position = vec3(0.0f, 5.0f, 0.0f),
         .orientation_euler_degrees = vec3(0.0f, 180.0f, 0.0f),
         .color = colors::kRed},
        {.position = vec3(10.0f, 5.0f, 0.0f),
         .orientation_euler_degrees = vec3(0.0f, 180.0f, 0.0f),
         .color = colors::kCyan},
        {.position = vec3(-10.0f, 5.0f, 0.0f),
         .orientation_euler_degrees = vec3(0.0f, 180.0f, 0.0f),
         .color = colors::kMagenta},
        {.position = vec3(-20.0f, 5.0f, 0.0f),
         .orientation_euler_degrees = vec3(0.0f, 180.0f, 0.0f),
         .color = colors::kYellow},
    };
}

void GameStateService::OnStart(ServiceProvider& service_provider)
{
    audio_service_ = &service_provider.GetService<AudioService>();

    GetEventBus().Subscribe<OnGuiEvent>(this);
}

void GameStateService::OnUpdate()
{
    const Timestep& delta_time = GetApp().GetDeltaTime();

    for (auto& t : timer_)
    {
        t.second += static_cast<float>(delta_time.GetSeconds());
    }

    active_powerups_ = PowerupsActive();
    RemoveActivePowerup();

    if (race_state_.state == GameState::kRaceInProgress)
    {
        race_state_.elapsed_time += delta_time;
    }
    else if (race_state_.state == GameState::kCountdown)
    {
        race_state_.countdown_elapsed_time += delta_time;

        if (race_state_.countdown_elapsed_time >= kCountdownTime)
        {
            race_state_.state = GameState::kRaceInProgress;
        }
    }
}

void GameStateService::OnGui()
{
    if (race_state_.state == GameState::kNotRunning)
    {
        return;
    }

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse;
    ImGui::SetNextWindowPos(ImVec2(40, 40));
    ImGui::Begin("Game State", nullptr, flags);

    if (race_state_.state == GameState::kCountdown)
    {
        ImGui::Text(
            "Countdown: %f sec",
            (kCountdownTime - race_state_.countdown_elapsed_time).GetSeconds());
    }
    else if (race_state_.state == GameState::kRaceInProgress)
    {
        ImGui::Text("Players: %zu", players_.size());
        ImGui::Text("Time: %.2f sec", race_state_.elapsed_time.GetSeconds());
    }
    else if (race_state_.state == GameState::kPostRace)
    {
        ImGui::Text("Finished!");
        ImGui::Text("Time: %f", race_state_.elapsed_time.GetSeconds());
    }

    ImGui::End();
}

void GameStateService::OnSceneLoaded(Scene& scene)
{
    num_checkpoints_ = 0;

    if (scene.GetName() == "Track1")
    {
        SetupRace();
        StartCountdown();
    }
}

void GameStateService::RemoveActivePowerup()
{
    // check if any of the powerup is over the limit assigned to them.
    for (auto& a : active_powerups_)
    {
        // finding the everyone slower powerup for the entity id.
        if (timer_.find(a) != timer_.end())
        {
            // check if it is over 5 seconds, set it back to 0 and remove the
            // active_powerup_ so that cars can go back to normal speed.
            if (a.second == PowerupPickupType::kEveryoneSlower)
            {
                if (timer_[a] > 5.f)
                {
                    for (int i = 0; i < active_powerups_.size(); i++)
                    {
                        if (active_powerups_[i].second ==
                            PowerupPickupType::kEveryoneSlower)
                        {
                            active_powerups_.erase(active_powerups_.begin() +
                                                   i);
                            same_powerup_.erase(a);
                            player_powers_.erase(a.first);

                            // to reset the powerup back to nothing. The player
                            // can pick up the new powerup now.
                            players_[a.first]
                                .state_component->SetCurrentPowerup(
                                    PowerupPickupType::kDefaultPowerup);

                            timer_.erase(a);
                        }
                    }
                }
            }
            else if (a.second == PowerupPickupType::kDisableHandling)
            {
                if (timer_[a] > 2.f)
                {
                    for (int i = 0; i < active_powerups_.size(); i++)
                    {
                        if (active_powerups_[i].second ==
                            PowerupPickupType::kDisableHandling)
                        {
                            active_powerups_.erase(active_powerups_.begin() +
                                                   i);
                            same_powerup_.erase(a);
                            player_powers_.erase(a.first);

                            // to reset the powerup back to nothing. The player
                            // can pick up the new powerup now.
                            players_[a.first]
                                .state_component->SetCurrentPowerup(
                                    PowerupPickupType::kDefaultPowerup);

                            timer_.erase(a);
                        }
                    }
                }
            }
            else if (a.second == PowerupPickupType::kKillAbilities)
            {
                for (int i = 0; i < active_powerups_.size(); i++)
                {
                    if (active_powerups_[i].second ==
                        PowerupPickupType::kKillAbilities)
                    {
                        active_powerups_.clear();
                        same_powerup_.erase(a);
                        player_powers_.erase(a.first);

                        // to reset the powerup back to nothing. The player
                        // can pick up the new powerup now.
                        players_[a.first].state_component->SetCurrentPowerup(
                            PowerupPickupType::kDefaultPowerup);

                        timer_.erase(a);
                    }
                }
            }
            else if (a.second == PowerupPickupType::kIncreaseAimBox)
            {
                if (timer_[a] > 4.f)
                {
                    for (int i = 0; i < active_powerups_.size(); i++)
                    {
                        if (active_powerups_[i].second ==
                            PowerupPickupType::kIncreaseAimBox)
                        {
                            active_powerups_.erase(active_powerups_.begin() +
                                                   i);
                            same_powerup_.erase(a);
                            player_powers_.erase(a.first);

                            // to reset the powerup back to nothing. The player
                            // can pick up the new powerup now.
                            players_[a.first]
                                .state_component->SetCurrentPowerup(
                                    PowerupPickupType::kDefaultPowerup);

                            timer_.erase(a);
                        }
                    }
                }
            }
        }
    }
}

void GameStateService::OnCleanup()
{
}

std::string_view GameStateService::GetName() const
{
    return "Game State Service";
}

std::vector<std::pair<uint32_t, PowerupPickupType>>
GameStateService::GetActivePowerups()
{
    return active_powerups_;
}

/**
 * @brief Makes a vector of powerups currently active by all the AIs and Player
 *
 * @return std::vector<PowerupPickupType>
 */
std::vector<std::pair<uint32_t, PowerupPickupType>>
GameStateService::PowerupsActive()
{
    std::vector<std::pair<uint32_t, PowerupPickupType>> powerups;
    for (auto& p : player_powers_)
    {
        if (p.second != PowerupPickupType::kDefaultPowerup)
        {
            powerups.push_back({p.first, p.second});

            // to check if the same powerup's timer doesnt start from 0 again.
            if (same_powerup_.find(p) == same_powerup_.end())
            {
                // starting the time of this powerup as well
                timer_.insert_or_assign({p.first, p.second}, 0.f);
                same_powerup_.insert(p);
            }
        }
    }
    return powerups;
}

void GameStateService::AddPlayerPowerup(uint32_t id, PowerupPickupType power)
{
    player_powers_.insert_or_assign(id, power);
}

void GameStateService::RemovePlayerPowerup(uint32_t id)
{
    player_powers_.insert_or_assign(id, PowerupPickupType::kDefaultPowerup);
}

uint32_t GameStateService::GetDisableHandlingMultiplier()
{
    // just return the ID which executed this powerup
    for (auto& a : active_powerups_)
    {
        if (a.second == PowerupPickupType::kDisableHandling)
        {
            return a.first;
        }
    }
    return NULL;
}

uint32_t GameStateService::GetEveryoneSlowerSpeedMultiplier()
{
    // just return the ID which executed this powerup
    for (auto& a : active_powerups_)
    {
        if (a.second == PowerupPickupType::kEveryoneSlower)
        {
            return a.first;
        }
    }
    return NULL;
}

uint32_t GameStateService::GetHitBoxMultiplier()
{
    // just return the ID which executed this powerup
    for (auto& a : active_powerups_)
    {
        if (a.second == PowerupPickupType::kIncreaseAimBox)
        {
            return a.first;
        }
    }
    return NULL;
}

void GameStateService::RemoveEveryoneSlowerSpeedMultiplier()
{
    for (int i = 0; i < active_powerups_.size(); i++)
    {
        if (active_powerups_[i].second == PowerupPickupType::kEveryoneSlower)
        {
            active_powerups_.erase(active_powerups_.begin() + i);
        }
    }
    std::cout << "active : " << active_powerups_.size();
}

void GameStateService::StartCountdown()
{
    // Reset global stats
    race_state_.state = GameState::kCountdown;
}

void GameStateService::SetupRace()
{
    race_state_.Reset();

    // Spawn players
    ASSERT_MSG(race_config_.num_ai_players + race_config_.num_human_players <=
                   kMaxPlayers,
               "Too many players added to the game");
    Log::info("Spawning players...");

    uint32_t player_idx = 0;

    for (uint32_t i = 0; i < race_config_.num_human_players; i++)
    {
        CreatePlayer(player_idx, true);
        player_idx++;
    }

    for (uint32_t i = 0; i < race_config_.num_ai_players; i++)
    {
        CreatePlayer(player_idx, false);
        player_idx++;
    }
}

void GameStateService::StartRace()
{
    race_state_.state = GameState::kRaceInProgress;
    Log::info("Game started");
}

void GameStateService::PlayerCompletedLap(PlayerRecord& player)
{
    if (race_state_.state != GameState::kRaceInProgress)
    {
        Log::error("Player finished lap before the game started");
        return;
    }

    const int laps = player.state_component->GetLapsCompleted() + 1;
    player.state_component->SetLapsCompleted(laps);

    if (laps == race_config_.num_laps)
    {
        Log::info("Player finished game!");
        audio_service_->PlayOneShot("yay.ogg");
        race_state_.finished_players++;
    }
}

void GameStateService::RegisterCheckpoint(Entity& entity)
{
    num_checkpoints_++;
}

void GameStateService::PlayerCrossedCheckpoint(Entity& entity, uint32_t index)
{
    auto iter = players_.find(entity.GetId());

    if (iter == players_.end())
    {
        return;
    }

    uint32_t last_checkpoint =
        iter->second.state_component->GetLastCheckpoint();
    uint32_t expected_checkpoint = last_checkpoint + 1;

    if (expected_checkpoint >= num_checkpoints_)
    {
        expected_checkpoint = 0;
    }

    if (index != expected_checkpoint)
    {
        return;
    }

    iter->second.state_component->SetLastCheckpoint(index);

    if (index == 0)
    {
        PlayerCompletedLap(iter->second);
    }
}

void GameStateService::SetRaceConfig(const RaceConfig& config)
{
    if (race_state_.state != GameState::kNotRunning)
    {
        Log::error("Cannot configure the race if it has already started");
        return;
    }

    race_config_ = config;
}

const RaceConfig& GameStateService::GetRaceConfig() const
{
    return race_config_;
}

const GlobalRaceState& GameStateService::GetGlobalRaceState() const
{
    return race_state_;
}

const uint32_t GameStateService::GetNumCheckpoints() const
{
    return num_checkpoints_;
}

Entity& GameStateService::CreatePlayer(uint32_t index, bool is_human)
{
    ASSERT_MSG(index <= kMaxPlayers, "Cannot have more players than max");
    ASSERT_MSG(players_.find(index) == players_.end(),
               "Cannot register multiple players with same index");

    Scene& scene = GetApp().GetSceneList().GetActiveScene();
    const PlayerSpawnConfig& config = track_config_.player_spawns[index];

    const std::string& entity_name =
        is_human ? kHumanPlayerNames[index] : kAiPlayerNames[index];

    // Create & configure car entity
    Entity& kart_entity = scene.AddEntity(entity_name);

    auto& transform = kart_entity.AddComponent<Transform>();
    transform.SetPosition(config.position);
    transform.RotateEulerDegrees(config.orientation_euler_degrees);

    auto& renderer = kart_entity.AddComponent<MeshRenderer>();
    renderer.SetMesh("kart2-4");
    renderer.SetMaterialProperties({.albedo_color = config.color,
                                    .specular = vec3(1.0f, 1.0f, 1.0f),
                                    .shininess = 64.0f});

    auto& player_state = kart_entity.AddComponent<PlayerState>();

    auto& vehicle = kart_entity.AddComponent<VehicleComponent>();
    vehicle.SetVehicleName(entity_name);
    vehicle.SetPlayerStateData(*player_state.GetStateData());

    auto& hitbox_component = kart_entity.AddComponent<Hitbox>();
    hitbox_component.SetSize(vec3(6.0f, 6.0f, 6.0f));

    kart_entity.AddComponent<RaycastComponent>();

    if (is_human)
    {
        kart_entity.AddComponent<PlayerController>();
        kart_entity.AddComponent<PlayerHud>();

        // Also create camera
        Entity& camera_entity = scene.AddEntity("PlayerCamera");
        camera_entity.AddComponent<Transform>();
        camera_entity.AddComponent<Camera>();

        auto& camera_follower = camera_entity.AddComponent<FollowCamera>();
        camera_follower.SetFollowingTransform(kart_entity);
    }
    else
    {
        auto& ai_controller = kart_entity.AddComponent<AIController>();
        ai_controller.SetGVehicle(vehicle.GetVehicle());
    }

    // Register the player
    players_[index] = {.index = index,
                       .entity = &kart_entity,
                       .state_component = &player_state};

    return kart_entity;
}