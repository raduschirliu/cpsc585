#include "GameStateService.h"

#include <imgui.h>

#include <algorithm>
#include <array>
#include <string>

#include "engine/App.h"
#include "engine/asset/AssetService.h"
#include "engine/audio/AudioService.h"
#include "engine/core/debug/Log.h"
#include "engine/gui/GuiService.h"
#include "engine/physics/BoxTrigger.h"
#include "engine/render/Camera.h"
#include "engine/render/MeshRenderer.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/SceneDebugService.h"
#include "game/Checkpoints.h"
#include "game/components/Controllers/AIController.h"
#include "game/components/Controllers/PlayerController.h"
#include "game/components/DebugCameraController.h"
#include "game/components/FollowCamera.h"
#include "game/components/Pickups/AmmoType/BuckshotPickup.h"
#include "game/components/Pickups/AmmoType/DoubleDamagePickup.h"
#include "game/components/Pickups/AmmoType/ExploadingBulletPickup.h"
#include "game/components/Pickups/AmmoType/IncreaseFireRatePickup.h"
#include "game/components/Pickups/AmmoType/VampireBulletPickup.h"
#include "game/components/Pickups/Powerups/DisableHandlingPickup.h"
#include "game/components/Pickups/Powerups/EveryoneSlowerPickup.h"
#include "game/components/Pickups/Powerups/IncreaseAimBoxPickup.h"
#include "game/components/Pickups/Powerups/KillAbilitiesPickup.h"
#include "game/components/VehicleComponent.h"
#include "game/components/audio/AudioEmitter.h"
#include "game/components/audio/AudioListener.h"
#include "game/components/race/Checkpoint.h"
#include "game/components/shooting/Hitbox.h"
#include "game/components/shooting/Shooter.h"
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

static const Timestep kCountdownTime = Timestep::Seconds(3.5);

static const array<string, kMaxPlayers> kCarTextures = {
    "kart@BodyMain-P1", "kart@BodyMain-P2", "kart@BodyMain-P3",
    "kart@BodyMain-P4"};
static const array<string, kMaxPlayers> kHumanPlayerNames = {
    "Player 1", "Player 2", "Player 3", "Player 4"};
static const array<string, kMaxPlayers> kAiPlayerNames = {"CPU 1", "CPU 2",
                                                          "CPU 3", "CPU 4"};
static const array<string, 5> kPowerups = {
    "kDefaultPowerup", "kDisableHandling", "kEveryoneSlower", "kIncreaseAimBox",
    "kKillAbilities"};

static const array<string, 6> kAmmos = {
    "kDefaultAmmo",      "kBuckshot",         "kDoubleDamage",
    "kExploadingBullet", "kIncreaseFireRate", "kVampireBullet"};

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

    // Get where the powerups should be spawned and what type.
    UpdatePowerupInfo();
}

void GameStateService::OnStart(ServiceProvider& service_provider)
{
    // Services
    audio_service_ = &service_provider.GetService<AudioService>();
    input_service_ = &service_provider.GetService<InputService>();
    asset_service_ = &service_provider.GetService<AssetService>();
    gui_service_ = &service_provider.GetService<GuiService>();
    scene_service_ = &service_provider.GetService<SceneDebugService>();
    input_service_ = &service_provider.GetService<InputService>();

    // Events
    GetEventBus().Subscribe<OnGuiEvent>(this);

    // Assets
    font_beya_ = gui_service_->GetFont("beya");
    font_pado_ = gui_service_->GetFont("pado");
    font_impact_ = gui_service_->GetFont("impact");
    font_cookie_ = gui_service_->GetFont("cookie");
    font_koverwatch_ = gui_service_->GetFont("koverwatch");
    font_mandu_ = gui_service_->GetFont("mandu");
    font_pixel_ = gui_service_->GetFont("pixel");

    countdown3_ = &asset_service_->GetTexture("countdown3");
    countdown2_ = &asset_service_->GetTexture("countdown2");
    countdown1_ = &asset_service_->GetTexture("countdown1");
    home_button_ = &asset_service_->GetTexture("home_button");
    ending_ = &asset_service_->GetTexture("ending");
    record_ = &asset_service_->GetTexture("record");
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
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoInputs;

    if (race_state_.state == GameState::kCountdown)
    {
        ImGui::SetNextWindowPos(ImVec2(425, 250));
        ImGui::Begin("Game State", nullptr, flags);
        double count =
            (kCountdownTime - race_state_.countdown_elapsed_time).GetSeconds();
        if (3 <= count && count < 4)
        {
            // ImGui::Text(
            //     "Countdown: %f sec",
            //     (kCountdownTime -
            //     race_state_.countdown_elapsed_time).GetSeconds());
            ImGui::Image(countdown3_->GetGuiHandle(), ImVec2(462, 227));
        }
        else if (2 <= count && count < 3)
        {
            ImGui::Image(countdown2_->GetGuiHandle(), ImVec2(462, 227));
        }
        else if (1 <= count && count < 2)
        {
            ImGui::Image(countdown1_->GetGuiHandle(), ImVec2(462, 227));
        }
        ImGui::End();
    }
    else if (race_state_.state == GameState::kRaceInProgress)
    {
        ImGui::SetNextWindowPos(ImVec2(220, 625));
        ImGui::Begin("Timer", nullptr, flags);

        ImGui::PushFont(font_beya_);
        int min = (int)race_state_.elapsed_time.GetSeconds() / 60;
        int second = (int)race_state_.elapsed_time.GetSeconds() % 60;
        ImGui::Text(
            "%02d:%02d:%02.0f", min, second,
            (race_state_.elapsed_time.GetSeconds() - (min * 60 + second)) *
                100);
        ImGui::PopFont();
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(1090, 610));
        ImGui::Begin("Ranking", nullptr, flags);
        for (size_t i = 0; i < race_state_.sorted_players.size(); i++)
        {
            // ImGui::Text("Players:", players_.size());
            // ImGui::Indent(10.0f);
            const int place = static_cast<int>(i + 1);
            Entity* entity = race_state_.sorted_players[i]->entity;
            // ImGui::Text("%d) %s", place, entity->GetName().c_str());
            PlayerState state = entity->GetComponent<PlayerState>();

            // ImGui::Text("health: %f", state.GetHealth());
            if (race_state_.sorted_players[i]->is_human)
            {
                ImGui::PushID(entity->GetId());
                ImGui::PushFont(font_pado_);
                if (place == 1)
                    ImGui::Text("%dst", place);
                else if (place == 2)
                    ImGui::Text("%dnd", place);
                else if (place == 3)
                    ImGui::Text("%drd", place);
                else if (place == 4)
                    ImGui::Text("%dth", place);
                ImGui::PopFont();
                ImGui::PopID();
            }
        }
        // ImGui::Unindent(10.0f);
        ImGui::End();
    }
    else if (race_state_.state == GameState::kPostRace)
    {
        // ImGui::SetNextWindowPos(ImVec2(0, 0));
        // ImGui::Begin("Game State", nullptr, flags);

        // ImGui::Text("Finished!");
        // ImGui::Text("Time: %f", race_state_.elapsed_time.GetSeconds());

        // ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::Begin("Background", nullptr, flags);
        ImGui::Image(
            ending_->GetGuiHandle(),
            ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y));

        ImGui::SetNextWindowPos(ImVec2(30, 200));
        ImGui::End();

        ImGui::Begin("Record", nullptr, flags);

        ImGui::Text("Finished!");
        ImGui::Text("Time: %f", race_state_.elapsed_time.GetSeconds());
        ImGui::End();
    }
    if (input_service_->IsKeyDown(GLFW_KEY_TAB))
    {
        DisplayScoreboard();
    }

    for (uint32_t i = 0; i < players_.size(); ++i)
    {
        Entity* entity = players_[i]->entity;

        if (least_deaths.second >
            player_details_[entity->GetId()].number_deaths)
        {
            least_deaths.first = entity->GetName();
            least_deaths.second =
                player_details_[entity->GetId()].number_deaths;
        }
        if (most_kills.second < player_details_[entity->GetId()].number_kills)
        {
            most_kills.first = entity->GetName();
            most_kills.second = player_details_[entity->GetId()].number_kills;
        }
    }

    ImGui::PushFont(font_mandu_);
    ImGui::Text("The least number of deaths");
    ImGui::Image(record_->GetGuiHandle(), ImVec2(75, 65));
    ImGui::SameLine(0.f, 50.0f);
    ImGui::Text("%s: %d", least_deaths.first.c_str(), least_deaths.second);
    ImGui::NewLine();
    ImGui::Text("The most number of kills");
    ImGui::Image(record_->GetGuiHandle(), ImVec2(75, 65));
    ImGui::SameLine(0.f, 50.0f);
    ImGui::Text("%s: %d", most_kills.first.c_str(), most_kills.second);
    ImGui::PopFont();

    ImGui::SetNextWindowPos(ImVec2(635, 270));
    ImGui::Begin("Result", nullptr, flags);

    for (size_t i = 0; i < race_state_.sorted_players.size(); i++)
    {
        const int place = static_cast<int>(i + 1);
        Entity* entity = race_state_.sorted_players[i]->entity;

        ImGui::PushID(entity->GetId());
        ImGui::PushFont(font_cookie_);
        if (place == 1 && race_state_.sorted_players[i]->is_human)
            ImGui::Text("%d\t%s\t", place, entity->GetName().c_str());
        else if (place == 1 && !race_state_.sorted_players[i]->is_human)
            ImGui::Text("%d\t%s\t\t", place, entity->GetName().c_str());
        else if (place == 2 && race_state_.sorted_players[i]->is_human)
            ImGui::Text("%d\t%s\t", place, entity->GetName().c_str());
        else if (place == 2 && !race_state_.sorted_players[i]->is_human)
            ImGui::Text("%d\t%s\t\t", place, entity->GetName().c_str());
        else if (place == 3 && race_state_.sorted_players[i]->is_human)
            ImGui::Text("%d\t%s\t", place, entity->GetName().c_str());
        else if (place == 3 && !race_state_.sorted_players[i]->is_human)
            ImGui::Text("%d\t%s\t\t", place, entity->GetName().c_str());
        else if (place == 4 && race_state_.sorted_players[i]->is_human)
            ImGui::Text("%d\t%s\t", place, entity->GetName().c_str());
        else if (place == 4 && !race_state_.sorted_players[i]->is_human)
            ImGui::Text("%d\t%s\t\t", place, entity->GetName().c_str());

        ImGui::SameLine(0.f, 100.f);
        int min = (int)race_state_.sorted_players[i]->finished_time / 60;
        int second = (int)race_state_.sorted_players[i]->finished_time % 60;
        ImGui::Text("%02d:%02d:%02.0f", min, second,
                    (race_state_.sorted_players[i]->finished_time -
                     (min * 60 + second)) *
                        100);
        ImGui::PopFont();
        ImGui::PopID();
    }
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 80,
                                   ImGui::GetIO().DisplaySize.y - 80));
    ImGui::Begin("home", nullptr, flags);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 50.0f);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.f, 1.f, 1.f, 0.1f));
    if (ImGui::ImageButton("home button", home_button_->GetGuiHandle(),
                           ImVec2(40, 37)))
    {
        scene_service_->SetActiveScene("MainMenu");
    }
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(1);

    if (input_service_->IsGamepadActive(GLFW_JOYSTICK_1) &&
        input_service_->IsGamepadButtonDown(GLFW_JOYSTICK_1,
                                            GLFW_GAMEPAD_BUTTON_LEFT_THUMB))
    {
        DisplayScoreboard();
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
                if (timer_[a] > 5.0f)
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
                if (timer_[a] > 2.0f)
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
                if (timer_[a] > 4.0f)
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
                timer_.insert_or_assign({p.first, p.second}, 0.0f);
                same_powerup_.insert(p);
            }
        }
    }
    return powerups;
}

void GameStateService::AddPlayerPowerup(uint32_t entity_id,
                                        PowerupPickupType power)
{
    // to tackle the problem for not changing the entity.
    if (entity_id >= 3 && entity_id <= 5)
    {
        entity_id = entity_id - 2;
    }
    // for player
    else if (entity_id == 1)
    {
        entity_id = entity_id - 1;
    }
    player_powers_.insert_or_assign(entity_id, power);
}

void GameStateService::RemovePlayerPowerup(uint32_t id)
{
    player_powers_.insert_or_assign(id, PowerupPickupType::kDefaultPowerup);
}

int GameStateService::GetDisableHandlingMultiplier()
{
    // just return the ID which executed this powerup
    for (auto& a : active_powerups_)
    {
        if (a.second == PowerupPickupType::kDisableHandling)
        {
            // due to how our player entities are set up :(
            if (a.first == 0)
            {
                return a.first + 1;
            }
            else
            {
                return a.first + 2;
            }
        }
    }
    return -1;
}

int GameStateService::GetEveryoneSlowerSpeedMultiplier()
{
    // just return the ID which executed this powerup
    for (auto& a : active_powerups_)
    {
        if (a.second == PowerupPickupType::kEveryoneSlower)
        {
            // due to how our player entities are set up :(
            if (a.first == 0)
            {
                return a.first + 1;
            }
            else
            {
                return a.first + 2;
            }
        }
    }
    return -1;
}

int GameStateService::GetHitBoxMultiplier()
{
    // just return the ID which executed this powerup
    for (auto& a : active_powerups_)
    {
        if (a.second == PowerupPickupType::kIncreaseAimBox)
        {
            // due to how our player entities are set up :(
            if (a.first == 0)
            {
                return a.first + 1;
            }
            else
            {
                return a.first + 2;
            }
        }
    }
    return -1;
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
    debug::LogDebug("active : {}", active_powerups_.size());
}

void GameStateService::StartCountdown()
{
    race_state_.state = GameState::kCountdown;
    debug::LogInfo("Race countdown started...");
}

void GameStateService::SetupRace()
{
    race_state_.Reset();

    // Spawn players
    ASSERT_MSG(race_config_.num_ai_players + race_config_.num_human_players <=
                   kMaxPlayers,
               "Too many players added to the game");
    debug::LogInfo("Spawning players...");

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

    SetupPowerups();
}

void GameStateService::SetupPowerups()
{
    Scene& scene = GetApp().GetSceneList().GetActiveScene();
    for (int i = 0, j = 0; i < powerup_info.size(), j < ammo_info_.size();
         i++, j++)
    {
    }
    for (const auto& powerup : powerup_info)
    {
        int powerup_to_int = 0;
        switch (powerup.first)
        {
            case PowerupPickupType::kDefaultPowerup:
                powerup_to_int = 0;
                break;

            case PowerupPickupType::kDisableHandling:
                powerup_to_int = 1;
                break;

            case PowerupPickupType::kEveryoneSlower:
                powerup_to_int = 2;
                break;

            case PowerupPickupType::kIncreaseAimBox:
                powerup_to_int = 3;
                break;

            case PowerupPickupType::kKillAbilities:
                powerup_to_int = 4;
                break;
        }
        // make entities for powerups here.
        string entity_name = kPowerups[powerup_to_int] + "  " +
                             std::to_string(powerup.second.x) + ", " +
                             std::to_string(powerup.second.y) + ", " +
                             std::to_string(powerup.second.z);

        Entity& entity = scene.AddEntity(entity_name);

        auto& transform = entity.AddComponent<Transform>();
        transform.SetPosition(powerup.second);

        auto& mesh_renderer = entity.AddComponent<MeshRenderer>();

        switch (powerup.first)
        {
            case PowerupPickupType::kDisableHandling:
                entity.AddComponent<DisableHandlingPickup>();
                mesh_renderer.SetMesh({
                    &asset_service_->GetMesh("coin"),
                    MaterialProperties{
                        .albedo_texture = nullptr,
                        .albedo_color = vec3(1.0f, 1.0f, 1.0f),
                        .specular = vec3(1.0f, 1.0f, 1.0f),
                        .shininess = 64.0f,
                    },
                });
                break;
            case PowerupPickupType::kEveryoneSlower:
                entity.AddComponent<EveryoneSlowerPickup>();
                mesh_renderer.SetMesh({
                    &asset_service_->GetMesh("energy"),
                    MaterialProperties{
                        .albedo_texture = nullptr,
                        .albedo_color = vec3(1.0f, 1.0f, 1.0f),
                        .specular = vec3(1.0f, 1.0f, 1.0f),
                        .shininess = 64.0f,
                    },
                });
                break;

            case PowerupPickupType::kIncreaseAimBox:
                entity.AddComponent<IncreaseAimBoxPickup>();
                mesh_renderer.SetMesh({
                    &asset_service_->GetMesh("defence"),
                    MaterialProperties{
                        .albedo_texture = nullptr,
                        .albedo_color = vec3(1.0f, 1.0f, 1.0f),
                        .specular = vec3(1.0f, 1.0f, 1.0f),
                        .shininess = 64.0f,
                    },
                });
                break;

            case PowerupPickupType::kKillAbilities:
                entity.AddComponent<KillAbilitiesPickup>();
                mesh_renderer.SetMesh({
                    &asset_service_->GetMesh("defence_shield"),
                    MaterialProperties{
                        .albedo_texture = nullptr,
                        .albedo_color = vec3(1.0f, 1.0f, 1.0f),
                        .specular = vec3(1.0f, 1.0f, 1.0f),
                        .shininess = 64.0f,
                    },
                });
                break;
        }
        transform.SetScale(vec3(0.12f, 0.12f, 0.12f));

        auto& trigger = entity.AddComponent<BoxTrigger>();
        trigger.SetSize(vec3(2.0f, 10.0f, 2.0f));
    }

    for (const auto& powerup : ammo_info_)
    {
        int powerup_to_int = 0;
        switch (powerup.first)
        {
            case AmmoPickupType::kDefaultAmmo:
                powerup_to_int = 0;
                break;

            case AmmoPickupType::kBuckshot:
                powerup_to_int = 1;
                break;

            case AmmoPickupType::kDoubleDamage:
                powerup_to_int = 2;
                break;

            case AmmoPickupType::kExploadingBullet:
                powerup_to_int = 3;
                break;

            case AmmoPickupType::kIncreaseFireRate:
                powerup_to_int = 4;
                break;

            case AmmoPickupType::kVampireBullet:
                powerup_to_int = 5;
                break;
        }
        // make entities for powerups here.
        string entity_name = kAmmos[powerup_to_int] + "  " +
                             std::to_string(powerup.second.x) + ", " +
                             std::to_string(powerup.second.y) + ", " +
                             std::to_string(powerup.second.z);

        Entity& entity = scene.AddEntity(entity_name);

        auto& transform = entity.AddComponent<Transform>();
        transform.SetPosition(powerup.second);

        auto& mesh_renderer = entity.AddComponent<MeshRenderer>();

        switch (powerup.first)
        {
            case AmmoPickupType::kBuckshot:
                entity.AddComponent<BuckshotPickup>();
                mesh_renderer.SetMesh({
                    &asset_service_->GetMesh("buckshot"),
                    MaterialProperties{
                        .albedo_texture = nullptr,
                        .albedo_color = vec3(1.0f, 1.0f, 1.0f),
                        .specular = vec3(1.0f, 1.0f, 1.0f),
                        .shininess = 64.0f,
                    },
                });
                break;
            case AmmoPickupType::kDoubleDamage:
                entity.AddComponent<DoubleDamagePickup>();
                mesh_renderer.SetMesh({
                    &asset_service_->GetMesh("damage"),
                    MaterialProperties{
                        .albedo_texture = nullptr,
                        .albedo_color = vec3(1.0f, 1.0f, 1.0f),
                        .specular = vec3(1.0f, 1.0f, 1.0f),
                        .shininess = 64.0f,
                    },
                });
                break;

            case AmmoPickupType::kExploadingBullet:
                entity.AddComponent<ExploadingBulletPickup>();
                mesh_renderer.SetMesh({
                    &asset_service_->GetMesh("exploding"),
                    MaterialProperties{
                        .albedo_texture = nullptr,
                        .albedo_color = vec3(1.0f, 1.0f, 1.0f),
                        .specular = vec3(1.0f, 1.0f, 1.0f),
                        .shininess = 64.0f,
                    },
                });
                break;

            case AmmoPickupType::kIncreaseFireRate:
                entity.AddComponent<IncreaseFireRatePickup>();
                mesh_renderer.SetMesh({
                    &asset_service_->GetMesh("increase"),
                    MaterialProperties{
                        .albedo_texture = nullptr,
                        .albedo_color = vec3(1.0f, 1.0f, 1.0f),
                        .specular = vec3(1.0f, 1.0f, 1.0f),
                        .shininess = 64.0f,
                    },
                });
                break;

            case AmmoPickupType::kVampireBullet:
                entity.AddComponent<VampireBulletPickup>();
                // mesh_renderer.SetMesh({
                //     &asset_service_->GetMesh("vampire"),
                //     MaterialProperties{
                //         .albedo_texture = nullptr,
                //         .albedo_color = vec3(1.0f, 1.0f, 1.0f),
                //         .specular = vec3(1.0f, 1.0f, 1.0f),
                //         .shininess = 64.0f,
                //     },
                // });
                break;
        }
        transform.SetScale(vec3(4.f, 4.f, 4.f));

        auto& trigger = entity.AddComponent<BoxTrigger>();
        trigger.SetSize(vec3(4.0f, 10.0f, 4.0f));
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

    debug::LogInfo("Game started");
}

void GameStateService::PlayerCompletedLap(PlayerRecord& player)
{
    if (race_state_.state != GameState::kRaceInProgress)
    {
        debug::LogError("Player finished lap before the game started");
        return;
    }

    debug::LogDebug("Player {}, completed the lap", player.entity->GetName());

    if (!player.is_human)
    {
        player.entity->GetComponent<AIController>().ResetForNextLap();
    }

    const int laps = player.state_component->GetLapsCompleted() + 1;
    player.state_component->SetLapsCompleted(laps);

    if (laps == race_config_.num_laps)
    {
        player.finished_time = race_state_.elapsed_time.GetSeconds();

        if (player.is_human)
        {
            debug::LogInfo("Player finished game!");
        }
        else
        {
            debug::LogInfo("AI finished game!");
        }

        audio_service_->AddSource("game_yay.ogg");
        audio_service_->PlaySource("game_yay.ogg");

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
    uint32_t entity_id = entity.GetId();
    // to tackle the problem for not changing the entity.
    if (entity_id >= 3 && entity_id <= 5)
    {
        entity_id = entity_id - 2;
    }
    // for player
    else if (entity_id == 1)
    {
        entity_id = entity_id - 1;
    }

    auto iter = players_.find(entity_id);

    if (iter == players_.end())
    {
        return;
    }

    // USE entity for FURTHER STUFF AS PLAYERS_ has wrong value for entty.

    // debug::LogInfo("Player {} hit  checkpoint {})",
    //                    iter->second->index, index);

    const uint32_t last_checkpoint =
        iter->second->state_component->GetLastCheckpoint();
    const uint32_t expected_checkpoint =
        (last_checkpoint + 1) % track_config_.checkpoints.size();

    if (index != expected_checkpoint)
    {
        debug::LogInfo("Player {} hit incorrect checkpoint {} (expected {})",
                       iter->second->index, index, expected_checkpoint);
        return;
    }

    iter->second->state_component->SetLastCheckpoint(index);
    iter->second->checkpoint_count_accumulator++;

    // debug::LogInfo("Player {} with id {} checkpoint accumulator #{}",
    //                iter->second->entity->GetName(), entity.GetName(),
    //                iter->second->checkpoint_count_accumulator);

    if (index == 0)
    {
        PlayerCompletedLap(*iter->second);
    }
}

void GameStateService::SetRaceConfig(const RaceConfig& config)
{
    if (race_state_.state != GameState::kNotRunning)
    {
        debug::LogError("Cannot configure the race if it has already started");
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
    renderer.SetMeshes({
        {
            &asset_service_->GetMesh("kart@BodyMain"),
            MaterialProperties{
                .albedo_texture =
                    &asset_service_->GetTexture(kCarTextures[index]),
                .albedo_color = vec3(1.0f, 1.0f, 1.0f),
                .specular = vec3(1.0f, 1.0f, 1.0f),
                .shininess = 64.0f,
            },
        },
        {
            &asset_service_->GetMesh("kart@BodyTop"),
            MaterialProperties{
                .albedo_texture = &asset_service_->GetTexture("kart@BodyTop"),
                .albedo_color = vec3(1.0f, 1.0f, 1.0f),
                .specular = vec3(1.0f, 1.0f, 1.0f),
                .shininess = 64.0f,
            },
        },
        {
            &asset_service_->GetMesh("kart@BodyUnderside"),
            MaterialProperties{
                .albedo_texture =
                    &asset_service_->GetTexture("kart@BodyUnderside"),
                .albedo_color = vec3(1.0f, 1.0f, 1.0f),
                .specular = vec3(1.0f, 1.0f, 1.0f),
                .shininess = 64.0f,
            },
        },
        {
            &asset_service_->GetMesh("kart@Muffler"),
            MaterialProperties{
                .albedo_texture = &asset_service_->GetTexture("kart@Muffler"),
                .albedo_color = vec3(1.0f, 1.0f, 1.0f),
                .specular = vec3(1.0f, 1.0f, 1.0f),
                .shininess = 64.0f,
            },
        },
        {
            &asset_service_->GetMesh("kart@Wheels"),
            MaterialProperties{
                .albedo_texture = &asset_service_->GetTexture("kart@Wheels"),
                .albedo_color = vec3(1.0f, 1.0f, 1.0f),
                .specular = vec3(1.0f, 1.0f, 1.0f),
                .shininess = 64.0f,
            },
        },
    });

    kart_entity.AddComponent<AudioEmitter>();

    auto& player_state = kart_entity.AddComponent<PlayerState>();

    auto& vehicle = kart_entity.AddComponent<VehicleComponent>();
    vehicle.SetVehicleName(entity_name);
    vehicle.SetPlayerStateData(*player_state.GetStateData());

    auto& hitbox_component = kart_entity.AddComponent<Hitbox>();
    hitbox_component.SetSize(vec3(15.0f, 10.0f, 15.0f));

    kart_entity.AddComponent<Shooter>();

    if (is_human)
    {
        auto& audio_listener = kart_entity.AddComponent<AudioListener>();

        kart_entity.AddComponent<PlayerController>();
        kart_entity.AddComponent<PlayerHud>();

        // Also create camera
        Entity& camera_entity = scene.AddEntity("PlayerCamera");
        camera_entity.AddComponent<Transform>();
        camera_entity.AddComponent<Camera>();

        auto& camera_follower = camera_entity.AddComponent<FollowCamera>();
        camera_follower.SetFollowingTransform(kart_entity);

        // camera_entity.AddComponent<DebugCameraController>();
    }
    else
    {
        auto& ai_controller = kart_entity.AddComponent<AIController>();
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

double GameStateService::GetMaxCountdownSeconds()
{
    return kCountdownTime.GetSeconds();
}

void GameStateService::DisplayScoreboard()
{
    ImGui::SetNextWindowPos(ImVec2(80, 40));
    ImGui::SetNextWindowSize(ImVec2(130, 100));

    auto flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders;

    if (ImGui::BeginTable("Scoreboard", 4, flags, ImVec2(250, 80), 250))
    {
        ImGui::TableSetupColumn("Player");
        ImGui::TableSetupColumn("Kills");
        ImGui::TableSetupColumn("Deaths");
        ImGui::TableSetupColumn("Laps Completed");
        ImGui::TableHeadersRow();
        ImGui::TableNextRow();

        for (auto& player : players_)
        {
            auto index = player.first;
            auto& state = player.second->state_component;
            if (player.second->is_human)
            {
                ImGui::TableNextColumn();
                ImGui::Text("Player %u", index + 1);
                ImGui::TableNextColumn();
                ImGui::Text("%d", state->GetKills());
                ImGui::TableNextColumn();
                ImGui::Text("%d", state->GetDeaths());
                ImGui::TableNextColumn();
                ImGui::Text("%d", state->GetLapsCompleted());
                ImGui::TableNextRow();
            }
            else
            {
                ImGui::TableNextColumn();
                ImGui::Text("CPU %u", index + 1);
                ImGui::TableNextColumn();
                ImGui::Text("%d", state->GetKills());
                ImGui::TableNextColumn();
                ImGui::Text("%d", state->GetDeaths());
                ImGui::TableNextColumn();
                ImGui::Text("%d", state->GetLapsCompleted());
                ImGui::TableNextRow();
            }
        }
        ImGui::EndTable();
    }
}

std::unordered_set<std::string> GameStateService::GetPlayerStaticNames()
{
    std::unordered_set<std::string> names;
    for (int i = 0; i < kHumanPlayerNames.size(); i++)
    {
        names.insert(kHumanPlayerNames[i]);
    }
    for (int i = 0; i < kAiPlayerNames.size(); i++)
    {
        names.insert(kAiPlayerNames[i]);
    }
    return names;
}

void GameStateService::UpdatePowerupInfo()
{
    // assigning powerup info here.

    // extract the information from obj file about every powerup location.
    Checkpoints checkpoints;
    auto locations = checkpoints.GetCheckpoints();

    // locations.second = orientation, we do not care about that, ignore that
    // for now

    for (const auto& l : locations)
    {
        // adding a random statement which will select to spawn ammo type or
        // powerup at the lcoation randomly generating what kind of powerup
        uint8_t selector = rand() % 2 + 1;
        if (selector == 1)
        {
            // means ammo should spawn
            uint8_t random_ammo = rand() % 5 + 1;
            switch (random_ammo)
            {
                case 1:
                    ammo_info_.push_back({AmmoPickupType::kBuckshot, l.first});
                    break;
                case 2:
                    ammo_info_.push_back(
                        {AmmoPickupType::kDoubleDamage, l.first});
                    break;
                case 3:
                    ammo_info_.push_back(
                        {AmmoPickupType::kExploadingBullet, l.first});
                    break;
                case 4:
                    ammo_info_.push_back(
                        {AmmoPickupType::kIncreaseFireRate, l.first});
                    break;
                case 5:
                    ammo_info_.push_back(
                        {AmmoPickupType::kVampireBullet, l.first});
                    break;
            }
        }
        else
        {
            // should spawn here.
            uint8_t random_powerup =
                rand() % 4 + 1;  // as 0 is default powerup.
            switch (random_powerup)
            {
                case 1:
                    powerup_info.push_back(
                        {PowerupPickupType::kDisableHandling, l.first});
                    break;
                case 2:
                    powerup_info.push_back(
                        {PowerupPickupType::kEveryoneSlower, l.first});
                    break;
                case 3:
                    powerup_info.push_back(
                        {PowerupPickupType::kIncreaseAimBox, l.first});
                    break;
                case 4:
                    powerup_info.push_back(
                        {PowerupPickupType::kKillAbilities, l.first});
                    break;
            }
        }
    }
}
