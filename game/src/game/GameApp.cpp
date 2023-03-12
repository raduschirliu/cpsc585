#include "game/GameApp.h"

#include <yaml-cpp/yaml.h>

#include <assimp/Importer.hpp>
#include <string>

#include "engine/AI/AIService.h"
#include "engine/asset/AssetService.h"
#include "engine/audio/AudioService.h"
#include "engine/audio/SoundEmitter.h"
#include "engine/config/ConfigService.h"
#include "engine/core/debug/Assert.h"
#include "engine/core/debug/Log.h"
#include "engine/game_state/GameStateService.h"
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
#include "engine/scene/SceneDebugService.h"
#include "engine/scene/Transform.h"
#include "game/components/BasicComponent.h"
#include "game/components/Controllers/AIController.h"
#include "game/components/Controllers/PlayerController.h"
#include "game/components/DebugCameraController.h"
#include "game/components/FinishLineComponent.h"
#include "game/components/FollowCamera.h"
#include "game/components/GuiExampleComponent.h"
#include "game/components/Pickups/Powerups/DisableHandlingPickup.h"
#include "game/components/Pickups/Powerups/EveryoneSlowerPickup.h"
#include "game/components/Pickups/Powerups/IncreaseAimBoxPickup.h"
#include "game/components/Pickups/Powerups/KillAbilitiesPickup.h"
#include "game/components/PlayerHud.h"
#include "game/components/RaycastComponent.h"
#include "game/components/VehicleComponent.h"
#include "game/components/state/PlayerState.h"

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

    AddService<ConfigService>();
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

    SetActiveScene("Track1");
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
}

void GameApp::LoadTestScene(Scene& scene)
{
    Log::info("Loading entities for TestScene...");

    {
        // Floor
        Entity& entity = scene.AddEntity("Floor");

        auto& transform = entity.AddComponent<Transform>();
        transform.SetPosition(vec3(0, 0, 0));
        transform.SetScale(vec3(250.0f, 1.0f, 250.0f));

        entity.AddComponent<PlaneStaticBody>();

        auto& mesh_renderer = entity.AddComponent<MeshRenderer>();
        mesh_renderer.SetMesh("cube");
        mesh_renderer.SetMaterialProperties(
            {.albedo_color = vec3(1.0f, 1.0f, 1.0f),
             .specular = vec3(1.0f, 1.0f, 1.0f),
             .shininess = 32.0f});
    }

    {
        // Cube
        // Entity& entity = scene.AddEntity("red cube");

        // Transform& transform = entity.AddComponent<Transform>();
        // transform.SetPosition(vec3(0.0, 5.0f, 0.0f));
        // transform.SetScale(vec3(5.0f, 5.0f, 5.0f));

        // auto& rigidbody = entity.AddComponent<BoxRigidBody>();
        // rigidbody.SetSize(vec3(5.0f, 5.0f, 5.0f));

        // auto& mesh_renderer = entity.AddComponent<MeshRenderer>();
        // mesh_renderer.SetMesh("cube");
        // mesh_renderer.SetMaterialProperties(
        //     {.albedo_color = vec3(1.0f, 0.2f, 0.2f),
        //      .specular = vec3(0.4f, 0.1f, 0.1f),
        //      .shininess = 128.0f});
    }

    //     auto& mesh_renderer = entity.AddComponent<MeshRenderer>();
    //     mesh_renderer.SetMesh("cube");
    //     mesh_renderer.SetMaterialProperties(
    //         {.albedo_color = vec3(1.0f, 0.2f, 0.2f),
    //          .specular = vec3(0.4f, 0.1f, 0.1f),
    //          .shininess = 128.0f});
    // }

    // auto& mesh_renderer = entity.AddComponent<MeshRenderer>();
    // mesh_renderer.SetMesh("cube");
    // mesh_renderer.SetMaterialProperties({.albedo_color = vec3(1.0f, 0.2f,
    // 0.2f),
    //                                      .specular = vec3(0.4f, 0.1f, 0.1f),
    //                                      .shininess = 128.0f});

    // {
    //     // Cube 2
    //     Entity& entity = scene.AddEntity("white cube");

    //     auto& transform = entity.AddComponent<Transform>();
    //     transform.SetPosition(vec3(10.0, 35.0f, 0.0f));
    //     transform.SetScale(vec3(1.0f, 3.0f, 1.0f));

    //     auto& rigidbody = entity.AddComponent<BoxRigidBody>();
    //     rigidbody.SetSize(vec3(1.0f, 3.0f, 1.0f));

    //     auto& mesh_renderer = entity.AddComponent<MeshRenderer>();
    //     mesh_renderer.SetMesh("cube");
    // }

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
        auto& raycast = car_entity.AddComponent<RaycastComponent>();

        auto& mesh_renderer = car_entity.AddComponent<MeshRenderer>();
        mesh_renderer.SetMesh("kart2-4");
        mesh_renderer.SetMaterialProperties(
            {.albedo_color = vec3(0.3f, 0.3f, 0.3f),
             .specular = vec3(0.3f, 0.3f, 0.3f),
             .shininess = 64.0f});

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
        // transform.RotateEulerDegrees(glm::vec3(0.f, -90.f, 0.f));
        //        transform.SetOrientation(glm::normalize(glm::quat(1.f,
        //        0.f, 1.f, 0.f)));
        auto& player_state = entity.AddComponent<PlayerState>();

        auto& bunny_vehicle = entity.AddComponent<VehicleComponent>();
        bunny_vehicle.SetVehicleName("AI1");
        bunny_vehicle.SetPlayerStateData(*player_state.GetStateData());
        auto& hitbox_component = entity.AddComponent<Hitbox>();
        hitbox_component.SetSize(vec3(10.f));
        auto& mesh_renderer = entity.AddComponent<MeshRenderer>();
        mesh_renderer.SetMesh("kart2-4");
        mesh_renderer.SetMaterialProperties(
            {.albedo_color = vec3(1.0f, 0.0f, 0.0f),
             .specular = vec3(1.0f, 0.0f, 0.0f),
             .shininess = 64.0f});

        // Making the controller which will guide the car on where to go
        auto& ai_controller = entity.AddComponent<AIController>();
        ai_controller.SetGVehicle(bunny_vehicle.GetVehicle());
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
        mesh_renderer.SetMesh("kart2-4");
        mesh_renderer.SetMaterialProperties(
            {.albedo_color = vec3(1.0f, 1.0f, 0.0f),
             .specular = vec3(0.0f, 1.0f, 0.0f),
             .shininess = 64.0f});

        // Making the controller which will guide the car on where to go
        auto& ai_controller = entity.AddComponent<AIController>();
        ai_controller.SetGVehicle(bunny_vehicle.GetVehicle());
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
        mesh_renderer.SetMesh("kart2-4");
        mesh_renderer.SetMaterialProperties(
            {.albedo_color = vec3(0.0f, 0.0f, 1.0f),
             .specular = vec3(0.0f, 0.0f, 1.0f),
             .shininess = 64.0f});

        // Making the controller which will guide the car on where to go
        auto& ai_controller = entity.AddComponent<AIController>();
        ai_controller.SetGVehicle(bunny_vehicle.GetVehicle());
    }

    {
        // Alleged finish line
        Entity& entity = scene.AddEntity("Finish Line");

        auto& transform = entity.AddComponent<Transform>();
        transform.SetPosition(vec3(50.0, 4.0f, 20.0f));
        transform.SetScale(vec3(10.0f, 4.0f, 10.0f));

        auto& trigger = entity.AddComponent<BoxTrigger>();
        trigger.SetSize(vec3(10.0f, 4.0f, 10.0f));

        entity.AddComponent<FinishLineComponent>();

        auto& mesh_renderer = entity.AddComponent<MeshRenderer>();
        mesh_renderer.SetMesh("cube");
        mesh_renderer.SetMaterialProperties(
            {.albedo_color = vec3(0.1f, 1.0f, 0.2f),
             .specular = vec3(1.0f, 1.0f, 1.0f),
             .shininess = 64.0f});
    }

    // // Make everyone slower pickup
    // {
    //     Entity& entity = scene.AddEntity("Slow Down Enemies");

    //     auto& transform = entity.AddComponent<Transform>();
    //     transform.SetPosition(vec3(0.0, 2.0f, 0.0f));

    //     auto& pickup = entity.AddComponent<EveryoneSlowerPickup>();
    //     auto& mesh_renderer = entity.AddComponent<MeshRenderer>();
    //     mesh_renderer.SetMesh("energy");
    //     // as it spawns way too big lol
    //     transform.SetScale(vec3(0.12, 0.12, 0.12));

    //     auto& trigger = entity.AddComponent<BoxTrigger>();
    //     trigger.SetSize(vec3(2.0f, 10.0f, 2.0f));
    // }

    // // Kill the abilities.
    // {
    //     Entity& entity = scene.AddEntity("Kill the abilities");

    //     auto& transform = entity.AddComponent<Transform>();
    //     transform.SetPosition(vec3(0.0, 2.0f, 0.0f));

    //     auto& pickup = entity.AddComponent<KillAbilitiesPickup>();
    //     auto& mesh_renderer = entity.AddComponent<MeshRenderer>();
    //     mesh_renderer.SetMesh("energy");
    //     // as it spawns way too big lol
    //     transform.SetScale(vec3(0.12, 0.12, 0.12));

    //     auto& trigger = entity.AddComponent<BoxTrigger>();
    //     trigger.SetSize(vec3(2.0f, 10.0f, 2.0f));
    // }

    // Disable Handling pickup
    {
        Entity& entity = scene.AddEntity("Disable Handling");

        auto& transform = entity.AddComponent<Transform>();
        transform.SetPosition(vec3(10.0f, 5.0f, -100.0f));
        auto& pickup = entity.AddComponent<DisableHandlingPickup>();
        auto& mesh_renderer = entity.AddComponent<MeshRenderer>();
        mesh_renderer.SetMesh("defence");
        // as it spawns way too big lol
        transform.SetScale(vec3(0.12, 0.12, 0.12));

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
        mesh_renderer.SetMesh("coin");
        // as it spawns way too big lol
        transform.SetScale(vec3(0.12, 0.12, 0.12));

        auto& trigger = entity.AddComponent<BoxTrigger>();
        trigger.SetSize(vec3(2.0f, 10.0f, 2.0f));
    }

    // // Kill the abilities pickup
    // {
    //     Entity& entity = scene.AddEntity("Kill abilities");

    //     auto& transform = entity.AddComponent<Transform>();
    //     transform.SetPosition(vec3(0.0f, 5.0f, -10.0f));
    //     auto& pickup = entity.AddComponent<IncreaseAimBoxPickup>();
    //     auto& mesh_renderer = entity.AddComponent<MeshRenderer>();
    //     mesh_renderer.SetMesh("defence_shield");
    //     // as it spawns way too big lol
    //     transform.SetScale(vec3(0.12, 0.12, 0.12));

    //     auto& trigger = entity.AddComponent<BoxTrigger>();
    //     trigger.SetSize(vec3(2.0f, 10.0f, 2.0f));
    // }
}

void GameApp::LoadTrack1Scene(Scene& scene)
{
    Log::info("Loading entities for Track1 scene...");

    {
        // Track
        auto& entity = scene.AddEntity("Track");

        auto& transform = entity.AddComponent<Transform>();
        transform.SetPosition(vec3(0.0f, 0.0f, 0.0f));

        auto& static_body = entity.AddComponent<MeshStaticBody>();
        static_body.SetMesh("track3-collision", 1.0f);

        auto& mesh_renderer = entity.AddComponent<MeshRenderer>();
        mesh_renderer.SetMesh("track3");
        mesh_renderer.SetMaterialProperties(
            {.albedo_color = vec3(1.0f, 1.0f, 1.0f),
             .specular = vec3(1.0f, 1.0f, 1.0f),
             .shininess = 32.0f});
    }
    {
        // Human player
        Entity& human_player =
            CreatePlayer(scene, "HumanPlayer", true, vec3(0.0f, 5.0f, 0.0f),
                         vec3(0.0f, 180.0f, 0.0f), colors::kRed);

        // Camera
        Entity& camera_entity = scene.AddEntity("DebugCamera");
        camera_entity.AddComponent<Transform>();
        camera_entity.AddComponent<Camera>();

        // camera_entity.AddComponent<DebugCameraController>();

        auto& camera_follower = camera_entity.AddComponent<FollowCamera>();
        camera_follower.SetFollowingTransform(human_player);
    }
    {
        // Other players
        CreatePlayer(scene, "AiPlayer1", false, vec3(10.f, 5.0, -1.f),
                     vec3(0.0f, 180.0f, 0.0f), colors::kCyan);
        CreatePlayer(scene, "AiPlayer2", false, vec3(-10.0f, 5.0f, 0.0f),
                     vec3(0.0f, 180.0f, 0.0f), colors::kMagenta);
        CreatePlayer(scene, "AiPlayer3", false, vec3(-20.0f, 5.0f, 0.0f),
                     vec3(0.0f, 180.0f, 0.0f), colors::kYellow);
    }

    {
        Entity& entity = scene.AddEntity("Powerup - Slow Down Enemies");

        auto& transform = entity.AddComponent<Transform>();
        transform.SetPosition(vec3(-500.0f, 2.0f, -475.0f));

        auto& pickup = entity.AddComponent<EveryoneSlowerPickup>();
        auto& mesh_renderer = entity.AddComponent<MeshRenderer>();
        mesh_renderer.SetMesh("energy");
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

        entity.AddComponent<FinishLineComponent>();

        auto& mesh_renderer = entity.AddComponent<MeshRenderer>();
        mesh_renderer.SetMesh("cube");
        mesh_renderer.SetMaterialProperties(
            {.albedo_color = vec3(0.1f, 1.0f, 0.2f),
             .specular = vec3(1.0f, 1.0f, 1.0f),
             .shininess = 64.0f});
    }
}

Entity& GameApp::CreatePlayer(Scene& scene, const string& name, bool human,
                              const vec3& position,
                              const vec3& orientation_euler_degrees,
                              const Color3& color)
{
    Entity& kart_entity = scene.AddEntity(name);

    auto& transform = kart_entity.AddComponent<Transform>();
    transform.SetPosition(position);
    transform.RotateEulerDegrees(orientation_euler_degrees);

    auto& renderer = kart_entity.AddComponent<MeshRenderer>();
    renderer.SetMesh("kart2-4");
    renderer.SetMaterialProperties({.albedo_color = color,
                                    .specular = vec3(1.0f, 1.0f, 1.0f),
                                    .shininess = 64.0f});

    auto& sound_emitter = kart_entity.AddComponent<SoundEmitter>();
    sound_emitter.AddSource("yay.ogg");

    auto& player_state = kart_entity.AddComponent<PlayerState>();

    auto& vehicle = kart_entity.AddComponent<VehicleComponent>();
    vehicle.SetVehicleName(name);
    vehicle.SetPlayerStateData(*player_state.GetStateData());

    auto& hitbox_component = kart_entity.AddComponent<Hitbox>();
    hitbox_component.SetSize(vec3(6.0f, 6.0f, 6.0f));

    kart_entity.AddComponent<RaycastComponent>();

    if (human)
    {
        kart_entity.AddComponent<PlayerController>();
        kart_entity.AddComponent<PlayerHud>();
    }
    else
    {
        auto& ai_controller = kart_entity.AddComponent<AIController>();
        ai_controller.SetGVehicle(vehicle.GetVehicle());
    }

    return kart_entity;
}
