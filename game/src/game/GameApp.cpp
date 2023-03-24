#include "game/GameApp.h"

#include <string>

#include "Checkpoints.h"
#include "engine/AI/AIService.h"
#include "engine/asset/AssetService.h"
#include "engine/audio/AudioService.h"
#include "engine/core/debug/Assert.h"
#include "engine/core/debug/Log.h"
#include "engine/gui/GuiService.h"
#include "engine/input/InputService.h"
#include "engine/physics/BoxRigidBody.h"
#include "engine/physics/BoxTrigger.h"
#include "engine/physics/Hitbox.h"
#include "engine/physics/MeshStaticBody.h"
#include "engine/physics/PhysicsService.h"
#include "engine/physics/PlaneStaticBody.h"
#include "engine/physics/SphereRigidBody.h"
#include "engine/render/Camera.h"
#include "engine/render/MeshRenderer.h"
#include "engine/render/RenderService.h"
#include "engine/scene/ComponentUpdateService.h"
#include "engine/scene/Scene.h"
#include "engine/scene/ScenedebugService.h"
#include "engine/scene/Transform.h"
#include "game/components/Controllers/AIController.h"
#include "game/components/Controllers/PlayerController.h"
#include "game/components/FollowCamera.h"
#include "game/components/Pickups/Powerups/DisableHandlingPickup.h"
#include "game/components/Pickups/Powerups/EveryoneSlowerPickup.h"
#include "game/components/Pickups/Powerups/IncreaseAimBoxPickup.h"
#include "game/components/Pickups/Powerups/KillAbilitiesPickup.h"
#include "game/components/Shooter.h"
#include "game/components/VehicleComponent.h"
#include "game/components/audio/AudioEmitter.h"
#include "game/components/audio/AudioListener.h"
#include "game/components/race/Checkpoint.h"
#include "game/components/state/PlayerState.h"
#include "game/components/ui/HowToPlay.h"
#include "game/components/ui/MainMenu.h"
#include "game/components/ui/PlayerHud.h"
#include "game/components/ui/Setting.h"
#include "game/services/GameStateService.h"

using glm::ivec2;
using glm::vec3;
using std::make_unique;
using std::string;

GameApp::GameApp()
{
}

/**
 * Runs once the windowing system has been initialized and the window
 * is created. Services should be added here.
 *
 * THIS IS CALLED BEFORE SERVICES ARE INITIALIZED
 */
void GameApp::OnInit()
{
    GetWindow().SetSize(ivec2(1280, 720));
    GetWindow().SetTitle("Angry Wheels");

    AddService<AssetService>();
    AddService<SceneDebugService>();
    AddService<InputService>();
    AddService<PhysicsService>();
    AddService<ComponentUpdateService>();
    AddService<RenderService>();
    AddService<AudioService>();
    AddService<GuiService>();
    AddService<AIService>();
    AddService<GameStateService>();
}

/**
 * Runs after all services have been initialized - it's safe to interact with
 * them here if needed
 */
void GameApp::OnStart()
{
    AddScene("Test");
    AddScene("Track1");
    AddScene("MainMenu");
    AddScene("HowToPlay");
    AddScene("Setting");

    SetActiveScene("MainMenu");
}

void GameApp::OnSceneLoaded(Scene& scene)
{
    const auto& scene_name = scene.GetName();

    if (scene_name == "Test")
    {
        LoadTestScene(scene);
    }
    else if (scene_name == "Track1")
    {
        LoadTrack1Scene(scene);
    }
    else if (scene_name == "MainMenu")
    {
        LoadMainMenuScene(scene);
    }
    else if (scene_name == "HowToPlay")
    {
        LoadHowToPlayScene(scene);
    }
    else if (scene_name == "Setting")
    {
        LoadSettingScene(scene);
    }
}

void GameApp::LoadTestScene(Scene& scene)
{
    debug::LogInfo("Loading entities for Test scene...");

    AssetService& asset_service_ =
        GetServiceProvider().GetService<AssetService>();

    {
        // Floor
        Entity& entity = scene.AddEntity("Floor");

        auto& transform = entity.AddComponent<Transform>();
        transform.SetPosition(vec3(0, 0, 0));
        transform.SetScale(vec3(250.0f, 1.0f, 250.0f));

        entity.AddComponent<PlaneStaticBody>();

        auto& mesh_renderer = entity.AddComponent<MeshRenderer>();
        mesh_renderer.SetMesh({
            &asset_service_.GetMesh("cube"),
            MaterialProperties{
                .albedo_texture = nullptr,
                .albedo_color = vec3(1.0f, 1.0f, 1.0f),
                .specular = vec3(1.0f, 1.0f, 1.0f),
                .shininess = 64.0f,
            },
        });
    }

    {
        // Player car
        Entity& car_entity = scene.AddEntity("PlayerVehicle");

        auto& transform = car_entity.AddComponent<Transform>();
        transform.SetPosition(vec3(10.0f, 5.0f, 10.0f));

        auto& player_state = car_entity.AddComponent<PlayerState>();

        auto& vehicle = car_entity.AddComponent<VehicleComponent>();
        vehicle.SetVehicleName("PlayerVehicle");

        vehicle.SetPlayerStateData(*player_state.GetStateData());

        auto& hitbox_component = car_entity.AddComponent<Hitbox>();
        hitbox_component.SetSize(vec3(10.f));

        auto& controller = car_entity.AddComponent<PlayerController>();
        auto& raycast = car_entity.AddComponent<Shooter>();

        auto& mesh_renderer = car_entity.AddComponent<MeshRenderer>();
        mesh_renderer.SetMesh({
            &asset_service_.GetMesh("kart-old"),
            MaterialProperties{
                .albedo_texture = nullptr,
                .albedo_color = vec3(0.3f, 0.3f, 0.3f),
                .specular = vec3(0.3f, 0.3f, 0.3f),
                .shininess = 64.0f,
            },
        });

        // Player camera following car
        Entity& camera_entity = scene.AddEntity("Camera");
        camera_entity.AddComponent<Transform>();
        camera_entity.AddComponent<Camera>();

        auto& camera_follower = camera_entity.AddComponent<FollowCamera>();
        camera_follower.SetFollowingTransform(car_entity);
    }

    {
        // AI 1
        Entity& entity = scene.AddEntity("AiVehicle1");

        auto& transform = entity.AddComponent<Transform>();
        transform.SetPosition(vec3(0.0, 5.0f, 10.0f));
        auto& player_state = entity.AddComponent<PlayerState>();

        auto& bunny_vehicle = entity.AddComponent<VehicleComponent>();
        bunny_vehicle.SetVehicleName("AI1");
        bunny_vehicle.SetPlayerStateData(*player_state.GetStateData());
        auto& hitbox_component = entity.AddComponent<Hitbox>();
        hitbox_component.SetSize(vec3(10.f));
        auto& mesh_renderer = entity.AddComponent<MeshRenderer>();
        mesh_renderer.SetMesh({
            &asset_service_.GetMesh("kart"),
            MaterialProperties{
                .albedo_texture = nullptr,
                .albedo_color = vec3(1.0f, 0.0f, 0.0f),
                .specular = vec3(1.0f, 0.0f, 0.0f),
                .shininess = 64.0f,
            },
        });
    }

    {
        // AI 2
        Entity& entity = scene.AddEntity("AiVehicle2");

        auto& transform = entity.AddComponent<Transform>();
        transform.SetPosition(vec3(20.0f, 5.0f, 10.0f));

        auto& player_state = entity.AddComponent<PlayerState>();

        auto& bunny_vehicle = entity.AddComponent<VehicleComponent>();
        bunny_vehicle.SetVehicleName("AI2");
        bunny_vehicle.SetPlayerStateData(*player_state.GetStateData());
        auto& hitbox_component = entity.AddComponent<Hitbox>();
        hitbox_component.SetSize(vec3(10.f));

        auto& mesh_renderer = entity.AddComponent<MeshRenderer>();
        mesh_renderer.SetMesh({
            &asset_service_.GetMesh("kart"),
            MaterialProperties{
                .albedo_texture = nullptr,
                .albedo_color = vec3(1.0f, 1.0f, 0.3f),
                .specular = vec3(1.0f, 1.0f, 0.3f),
                .shininess = 64.0f,
            },
        });
    }

    {
        // AI 3
        Entity& entity = scene.AddEntity("AiVehicle3");

        auto& transform = entity.AddComponent<Transform>();
        transform.SetPosition(vec3(30.0f, 5.0f, 10.0f));

        auto& player_state = entity.AddComponent<PlayerState>();

        auto& bunny_vehicle = entity.AddComponent<VehicleComponent>();
        bunny_vehicle.SetVehicleName("AI3");
        bunny_vehicle.SetPlayerStateData(*player_state.GetStateData());
        auto& hitbox_component = entity.AddComponent<Hitbox>();
        hitbox_component.SetSize(vec3(10.f));

        auto& mesh_renderer = entity.AddComponent<MeshRenderer>();
        mesh_renderer.SetMesh({
            &asset_service_.GetMesh("kart"),
            MaterialProperties{
                .albedo_texture = nullptr,
                .albedo_color = vec3(0.0f, 0.0f, 1.0f),
                .specular = vec3(0.0f, 0.0f, 1.0f),
                .shininess = 64.0f,
            },
        });
    }

    {
        // Alleged finish line
        Entity& entity = scene.AddEntity("Finish Line");

        auto& transform = entity.AddComponent<Transform>();
        transform.SetPosition(vec3(50.0, 4.0f, 20.0f));
        transform.SetScale(vec3(10.0f, 4.0f, 10.0f));

        auto& trigger = entity.AddComponent<BoxTrigger>();
        trigger.SetSize(vec3(10.0f, 4.0f, 10.0f));

        auto& mesh_renderer = entity.AddComponent<MeshRenderer>();
        mesh_renderer.SetMesh({
            &asset_service_.GetMesh("kart"),
            MaterialProperties{
                .albedo_texture = nullptr,
                .albedo_color = vec3(0.0f, 1.0f, 0.0f),
                .specular = vec3(0.0f, 1.0f, 0.0f),
                .shininess = 64.0f,
            },
        });
    }

    // Disable Handling pickup
    {
        Entity& entity = scene.AddEntity("Disable Handling");

        auto& transform = entity.AddComponent<Transform>();
        transform.SetPosition(vec3(10.0f, 5.0f, -100.0f));
        auto& pickup = entity.AddComponent<DisableHandlingPickup>();
        auto& mesh_renderer = entity.AddComponent<MeshRenderer>();
        mesh_renderer.SetMesh({
            &asset_service_.GetMesh("defence"),
            MaterialProperties{
                .albedo_texture = nullptr,
                .albedo_color = vec3(1.0f, 1.0f, 1.0f),
                .specular = vec3(1.0f, 1.0f, 1.0f),
                .shininess = 64.0f,
            },
        });
        transform.SetScale(vec3(0.12f, 0.12f, 0.12f));

        auto& trigger = entity.AddComponent<BoxTrigger>();
        trigger.SetSize(vec3(2.0f, 10.0f, 2.0f));
    }

    // Increase the size of aimbox
    {
        Entity& entity = scene.AddEntity("Aimbox increase");

        auto& transform = entity.AddComponent<Transform>();
        transform.SetPosition(vec3(-10.0f, 5.0f, -10.0f));
        auto& pickup = entity.AddComponent<IncreaseAimBoxPickup>();
        auto& mesh_renderer = entity.AddComponent<MeshRenderer>();
        mesh_renderer.SetMesh({
            &asset_service_.GetMesh("coin"),
            MaterialProperties{
                .albedo_texture = nullptr,
                .albedo_color = vec3(1.0f, 1.0f, 1.0f),
                .specular = vec3(1.0f, 1.0f, 1.0f),
                .shininess = 64.0f,
            },
        });
        transform.SetScale(vec3(0.12, 0.12, 0.12));

        auto& trigger = entity.AddComponent<BoxTrigger>();
        trigger.SetSize(vec3(2.0f, 10.0f, 2.0f));
    }
}

void GameApp::LoadTrack1Scene(Scene& scene)
{
    debug::LogInfo("Loading entities for Track1 scene...");

    AssetService& asset_service_ =
        GetServiceProvider().GetService<AssetService>();

    {
        // Track part with collision
        auto& entity = scene.AddEntity("Track-Main");

        auto& transform = entity.AddComponent<Transform>();
        transform.SetPosition(vec3(0.0f, 0.0f, 0.0f));

        auto& static_body = entity.AddComponent<MeshStaticBody>();
        static_body.SetMesh("track3-collision", 1.0f);

        auto& mesh_renderer = entity.AddComponent<MeshRenderer>();
        mesh_renderer.SetMeshes({
            {
                &asset_service_.GetMesh("track3@OrangeTrack"),
                MaterialProperties{
                    .albedo_texture =
                        &asset_service_.GetTexture("track3@OrangeTrack"),
                    .albedo_color = vec3(1.0f, 1.0f, 1.0f),
                    .specular = vec3(1.0f, 1.0f, 1.0f),
                    .shininess = 64.0f,
                },
            },
            {
                &asset_service_.GetMesh("track3@BlueTrack"),
                MaterialProperties{
                    .albedo_texture =
                        &asset_service_.GetTexture("track3@BlueTrack"),
                    .albedo_color = vec3(1.0f, 1.0f, 1.0f),
                    .specular = vec3(1.0f, 1.0f, 1.0f),
                    .shininess = 64.0f,
                },
            },
        });
    }

    {
        // Decorative track parts
        auto& entity = scene.AddEntity("Track-Decorative");

        auto& transform = entity.AddComponent<Transform>();
        transform.SetPosition(vec3(0.0f, 0.0f, 0.0f));

        auto& mesh_renderer = entity.AddComponent<MeshRenderer>();
        mesh_renderer.SetMeshes({
            {
                &asset_service_.GetMesh("track3@Blocks"),
                MaterialProperties{
                    .albedo_texture =
                        &asset_service_.GetTexture("track3@Blocks"),
                    .albedo_color = vec3(1.0f, 1.0f, 1.0f),
                    .specular = vec3(1.0f, 1.0f, 1.0f),
                    .shininess = 64.0f,
                },
            },
            {
                &asset_service_.GetMesh("track3@Globe"),
                MaterialProperties{
                    .albedo_texture =
                        &asset_service_.GetTexture("track3@Globe"),
                    .albedo_color = vec3(1.0f, 1.0f, 1.0f),
                    .specular = vec3(1.0f, 1.0f, 1.0f),
                    .shininess = 64.0f,
                },
            },
            {
                &asset_service_.GetMesh("track3@Screen"),
                MaterialProperties{
                    .albedo_texture =
                        &asset_service_.GetTexture("track3@Screen"),
                    .albedo_color = vec3(1.0f, 1.0f, 1.0f),
                    .specular = vec3(1.0f, 1.0f, 1.0f),
                    .shininess = 64.0f,
                },
            },
            {
                &asset_service_.GetMesh("track3@Rings2"),
                MaterialProperties{
                    .albedo_texture =
                        &asset_service_.GetTexture("track3@Rings"),
                    .albedo_color = vec3(1.0f, 1.0f, 1.0f),
                    .specular = vec3(1.0f, 1.0f, 1.0f),
                    .shininess = 64.0f,
                },
            },
            {
                &asset_service_.GetMesh("track3@Rings6"),
                MaterialProperties{
                    .albedo_texture =
                        &asset_service_.GetTexture("track3@Rings"),
                    .albedo_color = vec3(1.0f, 1.0f, 1.0f),
                    .specular = vec3(1.0f, 1.0f, 1.0f),
                    .shininess = 64.0f,
                },
            },
        });
    }

    {
        Entity& entity = scene.AddEntity("Powerup - Slow Down Enemies");

        auto& transform = entity.AddComponent<Transform>();
        transform.SetPosition(vec3(-500.0f, 2.0f, -475.0f));

        auto& pickup = entity.AddComponent<EveryoneSlowerPickup>();
        auto& mesh_renderer = entity.AddComponent<MeshRenderer>();
        mesh_renderer.SetMesh({
            &asset_service_.GetMesh("energy"),
            MaterialProperties{
                .albedo_texture = nullptr,
                .albedo_color = vec3(1.0f, 1.0f, 1.0f),
                .specular = vec3(1.0f, 1.0f, 1.0f),
                .shininess = 64.0f,
            },
        });
        transform.SetScale(vec3(0.12f, 0.12f, 0.12f));

        auto& trigger = entity.AddComponent<BoxTrigger>();
        trigger.SetSize(vec3(2.0f, 10.0f, 2.0f));
    }
    {
        // Alleged finish line
        Entity& entity = scene.AddEntity("Finish Line");

        auto& transform = entity.AddComponent<Transform>();
        transform.SetPosition(vec3(10.0, 2.0f, 50.0f));
        transform.SetScale(vec3(40.0f, 5.0f, 4.0f));

        auto& trigger = entity.AddComponent<BoxTrigger>();
        trigger.SetSize(vec3(40.0f, 4.0f, 10.0f));

        auto& checkpoint = entity.AddComponent<Checkpoint>();
        checkpoint.SetCheckpointIndex(0);

        auto& mesh_renderer = entity.AddComponent<MeshRenderer>();
        mesh_renderer.SetMesh({
            &asset_service_.GetMesh("cube"),
            MaterialProperties{
                .albedo_texture = nullptr,
                .albedo_color = vec3(0.0f, 1.0f, 0.0f),
                .specular = vec3(0.0f, 1.0f, 0.0f),
                .shininess = 64.0f,
            },
        });
    }

    // making the object for the checkpoint and getting checkpoints.
    Checkpoints checkpoints_obj;
    auto checkpoints = checkpoints_obj.GetCheckpoints();

    for (int i = 0; i < checkpoints.size(); i++)
    {
        Entity& entity = scene.AddEntity("Checkpoint " + std::to_string(i));
        glm::quat(glm::vec3(0.0f));
        auto& transform = entity.AddComponent<Transform>();
        transform.SetPosition(checkpoints[i].first);
        transform.SetOrientation(glm::quat(checkpoints[i].second));

        transform.SetScale(vec3(70.0f, 10.0f, 10.0f));
        auto& trigger = entity.AddComponent<BoxTrigger>();
        trigger.SetSize(vec3(70.0f, 10.0f, 10.0f));

        auto& checkpoint = entity.AddComponent<Checkpoint>();
        checkpoint.SetCheckpointIndex(i + 2);
    }
}

void GameApp::LoadMainMenuScene(Scene& scene)
{
    debug::LogInfo("Loading entities for MainMenu scene...");

    Entity& entity = scene.AddEntity("Menu");
    entity.AddComponent<MainMenu>();
}

void GameApp::LoadHowToPlayScene(Scene& scene)
{
    debug::LogInfo("Loading entities for HowToPlay scene...");

    Entity& entity = scene.AddEntity("HowToPlay");
    entity.AddComponent<HowToPlay>();
}

void GameApp::LoadSettingScene(Scene& scene)
{
    debug::LogInfo("Loading entities for Setting scene...");

    Entity& entity = scene.AddEntity("Setting");
    entity.AddComponent<Setting>();
}
