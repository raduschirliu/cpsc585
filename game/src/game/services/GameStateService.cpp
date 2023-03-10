#include "GameStateService.h"

#include <imgui.h>

#include "engine/App.h"
#include "engine/core/debug/Log.h"
#include "engine/scene/OnUpdateEvent.h"
#include "game/components/state/PlayerState.h"

// so that as soon as 5 seconds are hit the
// powerup is disabled and removed.
static constexpr float kSlowDownTimerLimit = 5.0f;

static const Timestep kCountdownTime = Timestep::Seconds(12.0);
static const Timestep kMinRaceTime = Timestep::Seconds(50.0);

void GameStats::Reset()
{
    state = GameState::kNotRunning;
    countdown_elapsed_time.SetSeconds(0);
    elapsed_time.SetSeconds(0);
    finished_players = 0;
    num_laps = 0;
}

GameStateService::GameStateService()
{
}

void GameStateService::OnInit()
{
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

    if (stats_.state == GameState::kRunning)
    {
        stats_.elapsed_time += delta_time;
    }
    else if (stats_.state == GameState::kCountdown)
    {
        stats_.countdown_elapsed_time += delta_time;

        if (stats_.countdown_elapsed_time >= kCountdownTime)
        {
            StartGame();
        }
    }
}

void GameStateService::OnGui()
{
    if (stats_.state == GameState::kNotRunning)
    {
        return;
    }

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse;
    ImGui::SetNextWindowPos(ImVec2(40, 40));
    ImGui::Begin("Game State", nullptr, flags);

    if (stats_.state == GameState::kCountdown)
    {
        ImGui::Text(
            "Countdown: %f sec",
            (kCountdownTime - stats_.countdown_elapsed_time).GetSeconds());
    }
    else if (stats_.state == GameState::kRunning)
    {
        ImGui::Text("Players: %zu", players_.size());
        ImGui::Text("Time: %.2f sec", stats_.elapsed_time.GetSeconds());
    }
    else if (stats_.state == GameState::kFinished)
    {
        ImGui::Text("Finished!");
        ImGui::Text("Time: %f", stats_.elapsed_time.GetSeconds());
    }

    ImGui::End();
}

void GameStateService::OnSceneLoaded(Scene& scene)
{
    stats_.Reset();
    num_checkpoints_ = 0;
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

void GameStateService::RegisterPlayer(uint32_t id, Entity& entity,
                                      PlayerState* player_state)
{
    if (players_.find(id) != players_.end())
    {
        Log::warn(
            "Player with ID {} has been registered twice in GameStateService",
            id);
    }

    players_[id] = {.entity = &entity, .state_component = player_state};

    // TODO: This shouldn't be hardcoded to 4
    if (players_.size() == 4)
    {
        StartCountdown();
    }
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
    stats_.state = GameState::kCountdown;
    stats_.num_laps = 1;

    // Reset player stats
    auto iter = players_.begin();
    while (iter != players_.end())
    {
        iter->second.state_component->SetLapsCompleted(0);
        iter->second.state_component->SetLastCheckpoint(0);

        iter++;
    }
}

void GameStateService::StartGame()
{
    stats_.state = GameState::kRunning;
}

void GameStateService::PlayerCompletedLap(PlayerRecord& player)
{
    if (stats_.state != GameState::kRunning)
    {
        Log::error("Player finished lap before the game started");
        return;
    }

    int laps = player.state_component->GetLapsCompleted() + 1;
    player.state_component->SetLapsCompleted(laps);

    if (laps == stats_.num_laps)
    {
        Log::info("Player finished game!");
        audio_service_->PlayOneShot("yay.ogg");
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

const GameStats& GameStateService::GetGameStats() const
{
    return stats_;
}

const uint32_t GameStateService::GetNumCheckpoints() const
{
    return num_checkpoints_;
}
