#include "GameStateService.h"

#include <imgui.h>

#include "engine/App.h"
#include "engine/core/debug/Log.h"
#include "engine/scene/OnUpdateEvent.h"
#include "game/components/state/PlayerState.h"

static constexpr float kSlowDownTimerLimit(
    5.f);  // so that as soon as 5 seconds are hit the
           // powerup is disabled and removed.

static const Timestep kCountdownTime = Timestep::Seconds(12.0);
static const Timestep kMinRaceTime = Timestep::Seconds(50.0);

void GameStats::Reset()
{
    state = GameState::kNotRunning;
    countdown_elapsed_time.SetSeconds(0);
    elapsed_time.SetSeconds(0);
    finished_players = 0;
    num_laps = 0;
    num_players = 0;
}

GameStateService::GameStateService()
{
}

void GameStateService::OnInit()
{
    Log::debug("Game State Service working");
}

void GameStateService::OnStart(ServiceProvider& service_provider)
{
    GetEventBus().Subscribe<OnGuiEvent>(this);
}

void GameStateService::OnUpdate()
{
    const Timestep& delta_time = GetApp().GetDeltaTime();

    // get the powerups which are active right now.

    // increment all the timer values.
    for (auto& t : timer_)
    {
        // Log::debug("{}", t.second);
        t.second += delta_time.GetSeconds();
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
        ImGui::Text("Laps: %zu", stats_.num_laps);
        ImGui::Text("Players: %zu", stats_.num_players);
        ImGui::Text("Time: %f sec", stats_.elapsed_time.GetSeconds());
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

    if (scene.GetName() == "Track1")
    {
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
                            player_states_[a.first]->SetCurrentPowerup(
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
                            player_states_[a.first]->SetCurrentPowerup(
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
                        player_states_[a.first]->SetCurrentPowerup(
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
                            player_states_[a.first]->SetCurrentPowerup(
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

void GameStateService::AddPlayerDetails(uint32_t id, PlayerStateData details)
{
    player_details_.insert_or_assign(id, details);
}

void GameStateService::AddPlayerStates(uint32_t id, PlayerState* states)
{
    if (states)
    {
        player_states_.insert_or_assign(id, states);
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
            return a.first;
    }
    return NULL;
}

uint32_t GameStateService::GetEveryoneSlowerSpeedMultiplier()
{
    // just return the ID which executed this powerup
    for (auto& a : active_powerups_)
    {
        if (a.second == PowerupPickupType::kEveryoneSlower)
            return a.first;
    }
    return NULL;
}

uint32_t GameStateService::GetHitBoxMultiplier()
{
    // just return the ID which executed this powerup
    for (auto& a : active_powerups_)
    {
        if (a.second == PowerupPickupType::kIncreaseAimBox)
            return a.first;
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
    stats_.state = GameState::kCountdown;
}

void GameStateService::StartGame()
{
    stats_.state = GameState::kRunning;
}

void GameStateService::PlayerFinished(Entity& entity)
{
    if (stats_.state != GameState::kRunning)
    {
        return;
    }

    if (stats_.elapsed_time >= kMinRaceTime)
    {
        Log::info("Player finished: {}", entity.GetName());
        stats_.state = GameState::kFinished;
    }
    else
    {
        Log::info("Stop trying to cheat smfh");
    }
}