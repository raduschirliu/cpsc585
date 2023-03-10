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
#include "game/components/RaycastComponent.h"
#include "game/components/VehicleComponent.h"
#include "game/components/race/Checkpoint.h"
#include "game/components/state/PlayerState.h"
#include "game/components/ui/PlayerHud.h"

using glm::vec3;
using std::array;
using std::make_unique;
using std::string;

// so that as soon as 5 seconds are hit the
// powerup is disabled and removed.
static constexpr float kSlowDownTimerLimit = 5.0f;
static constexpr uint32_t kMaxPlayers = 4;

static const Timestep kCountdownTime = Timestep::Seconds(7.0);

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
    sorted_players.clear();
}

GameStateService::GameStateService()
{
}

void GameStateService::OnInit()
{
    race_config_.num_human_players = 1;
    race_config_.num_ai_players = 3;
    race_config_.num_laps = 2;

    race_state_.Reset();
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

    UpdateRaceTimer(delta_time);
    UpdatePlayerProgressScore(delta_time);
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
        ImGui::Text("Time: %.2f sec", race_state_.elapsed_time.GetSeconds());
        ImGui::Text("Players:", players_.size());
        ImGui::Indent(10.0f);

        for (size_t i = 0; i < race_state_.sorted_players.size(); i++)
        {
            const int place = static_cast<int>(i + 1);
            Entity* entity = race_state_.sorted_players[i]->entity;

            ImGui::PushID(entity->GetId());
            ImGui::Text("%d) %s", place, entity->GetName().c_str());
            ImGui::PopID();
        }

        ImGui::Unindent(10.0f);
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
    race_state_.Reset();
    track_config_.Reset();
    players_.clear();

    if (scene.GetName() == "Track1")
    {
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
                                ->state_component->SetCurrentPowerup(
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
                                ->state_component->SetCurrentPowerup(
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
                        players_[a.first]->state_component->SetCurrentPowerup(
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
                                ->state_component->SetCurrentPowerup(
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
    race_state_.state = GameState::kCountdown;
    Log::info("Race countdown started...");
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
    race_state_.elapsed_time.SetSeconds(0.0);
    race_state_.finished_players = 0;

    for (auto& entry : players_)
    {
        race_state_.sorted_players.push_back(entry.second.get());
    }

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

void GameStateService::RegisterCheckpoint(Entity& entity,
                                          Checkpoint* checkpoint)
{
    CheckpointRecord new_record = {
        .index = checkpoint->GetCheckpointIndex(),
        .entity = &entity,
        .position = entity.GetComponent<Transform>().GetPosition(),
        .distance_to_next = 0.0f};

    if (track_config_.checkpoints.size() > 0)
    {
        CheckpointRecord& first_record = track_config_.checkpoints.front();
        CheckpointRecord& last_record = track_config_.checkpoints.back();

        last_record.distance_to_next =
            glm::distance(last_record.position, new_record.position);
        new_record.distance_to_next =
            glm::distance(new_record.position, first_record.position);
    }

    track_config_.checkpoints.push_back(new_record);
}

void GameStateService::PlayerCrossedCheckpoint(Entity& entity, uint32_t index)
{
    auto iter = players_.find(entity.GetId());

    if (iter == players_.end())
    {
        return;
    }

    const uint32_t last_checkpoint =
        iter->second->state_component->GetLastCheckpoint();
    const uint32_t expected_checkpoint =
        (last_checkpoint + 1) % track_config_.checkpoints.size();

    if (index != expected_checkpoint)
    {
        Log::info("Player {} hit incorrect checkpoint {} (expected {})",
                  iter->second->index, index, expected_checkpoint);
        return;
    }

    iter->second->state_component->SetLastCheckpoint(index);
    iter->second->checkpoint_count_accumulator++;

    if (index == 0)
    {
        PlayerCompletedLap(*iter->second);
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
    return static_cast<uint32_t>(track_config_.checkpoints.size());
}

void GameStateService::UpdateRaceTimer(const Timestep& delta_time)
{
    if (race_state_.state == GameState::kRaceInProgress)
    {
        race_state_.elapsed_time += delta_time;
    }
    else if (race_state_.state == GameState::kCountdown)
    {
        race_state_.countdown_elapsed_time += delta_time;

        if (race_state_.countdown_elapsed_time >= kCountdownTime)
        {
            StartRace();
        }
    }
}

void GameStateService::UpdatePlayerProgressScore(const Timestep& delta_time)
{
    if (race_state_.state != GameState::kRaceInProgress)
    {
        return;
    }

    for (auto& entry : players_)
    {
        auto player = entry.second.get();

        const float checkpoint_progress =
            static_cast<float>(player->checkpoint_count_accumulator) * 10000.0f;

        const int last_checkpoint_index =
            player->state_component->GetLastCheckpoint();
        ASSERT_MSG(last_checkpoint_index >= 0 &&
                       last_checkpoint_index < track_config_.checkpoints.size(),
                   "Player must have valid checkpoint index");
        const CheckpointRecord& last_checkpoint =
            track_config_.checkpoints[last_checkpoint_index];
        const CheckpointRecord& next_checkpoint =
            GetNextCheckpoint(last_checkpoint_index + 1);

        const float dist_to_next = glm::distance(
            player->transform->GetPosition(), next_checkpoint.position);

        // Value [0.0f, 100.0f] represententing how far a player is to the next
        // checkpoint
        const float current_checkpoint_progress =
            glm::max(last_checkpoint.distance_to_next - dist_to_next, 0.0f) /
            last_checkpoint.distance_to_next * 100.0f;

        player->progress_score =
            checkpoint_progress + current_checkpoint_progress;
    }

    // Sort players in descending order
    std::sort(race_state_.sorted_players.begin(),
              race_state_.sorted_players.end(),
              [](PlayerRecord* a, PlayerRecord* b)
              { return a->progress_score > b->progress_score; });

    for (int i = 0; i < race_state_.sorted_players.size(); i++)
    {
        race_state_.sorted_players[i]->state_component->SetCurrentPlace(i);
    }
}

Entity& GameStateService::CreatePlayer(uint32_t index, bool is_human)
{
    ASSERT_MSG(index <= kMaxPlayers, "Cannot have more players than max");
    ASSERT_MSG(index < track_config_.player_spawns.size(),
               "Cannot have more players than number of spawn locations");
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
    players_[index] = make_unique<PlayerRecord>(
        PlayerRecord{.index = index,
                     .is_human = is_human,
                     .entity = &kart_entity,
                     .transform = &transform,
                     .state_component = &player_state,
                     .checkpoint_count_accumulator = 0,
                     .progress_score = 0.0f});

    return kart_entity;
}

CheckpointRecord& GameStateService::GetNextCheckpoint(uint32_t current_index)
{
    size_t next_index = static_cast<size_t>(current_index) + 1;
    next_index %= track_config_.checkpoints.size();

    return track_config_.checkpoints[next_index];
}