#include "game/GameApp.h"

#include <yaml-cpp/yaml.h>

#include <assimp/Importer.hpp>
#include <string>

#include "engine/asset/AssetService.h"
#include "engine/config/ConfigService.h"
#include "engine/core/debug/Assert.h"
#include "engine/core/debug/Log.h"
#include "engine/gui/GuiService.h"
#include "engine/input/InputService.h"
#include "engine/physics/BoxRigidBody.h"
#include "engine/physics/PhysicsService.h"
#include "engine/physics/PlaneStaticBody.h"
#include "engine/physics/SphereRigidBody.h"
#include "engine/render/Camera.h"
#include "engine/render/MeshRenderer.h"
#include "engine/render/RenderService.h"
#include "engine/scene/ComponentUpdateService.h"
#include "engine/scene/Scene.h"
#include "engine/scene/Transform.h"
#include "game/components/BasicComponent.h"
#include "game/components/DebugCameraController.h"
#include "game/components/GuiExampleComponent.h"
#include "game/components/VehicleComponent.h"

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

    AddService<ConfigService>();
    AddService<InputService>();
    AddService<PhysicsService>();
    AddService<ComponentUpdateService>();
    AddService<RenderService>();
    AddService<GuiService>();
    AddService<AssetService>();
}

/**
 * Runs after all services have been initialized - it's safe to interact with
 * them here if needed
 */
void GameApp::OnStart()
{
    Scene& scene = AddScene("TestScene");
    SetActiveScene("TestScene");

    {
        // Camera
        Entity& entity = scene.AddEntity();

        auto& transform = entity.AddComponent<Transform>();
        transform.SetPosition(vec3(0.0f, 10.0f, 15.0f));

        entity.AddComponent<Camera>();
        entity.AddComponent<DebugCameraController>();
    }

    {
        // Gui Example
        Entity& entity = scene.AddEntity();

        auto& transform = entity.AddComponent<Transform>();
        transform.SetPosition(vec3(10.0f, 0.0f, 0.0f));

        entity.AddComponent<GuiExampleComponent>();
    }

    {
        // Floor
        Entity& entity = scene.AddEntity();

        auto& transform = entity.AddComponent<Transform>();
        transform.SetPosition(vec3(0, 0, 0));
        transform.SetScale(vec3(10.0f, 1.0f, 10.0f));
        transform.RotateEulerDegrees(vec3(-90.0f, 0.0f, 0.0f));

        entity.AddComponent<PlaneStaticBody>();

        auto& mesh_renderer = entity.AddComponent<MeshRenderer>();
        mesh_renderer.SetMesh("plane");
    }

    {
        // Cube
        Entity& entity = scene.AddEntity();

        Transform& transform = entity.AddComponent<Transform>();
        transform.SetPosition(vec3(0.0, 5.0f, 0.0f));
        transform.SetScale(vec3(5.0f, 5.0f, 5.0f));

        auto& rigidbody = entity.AddComponent<BoxRigidBody>();
        rigidbody.SetSize(vec3(5.0f, 5.0f, 5.0f));

        auto& mesh_renderer = entity.AddComponent<MeshRenderer>();
        mesh_renderer.SetMesh("cube");
    }

    {
        // Cube 2
        Entity& entity = scene.AddEntity();

        auto& transform = entity.AddComponent<Transform>();
        transform.SetPosition(vec3(10.0, 35.0f, 0.0f));
        transform.SetScale(vec3(1.0f, 3.0f, 1.0f));

        auto& rigidbody = entity.AddComponent<BoxRigidBody>();
        rigidbody.SetSize(vec3(1.0f, 3.0f, 1.0f));

        auto& mesh_renderer = entity.AddComponent<MeshRenderer>();
        mesh_renderer.SetMesh("cube");
    }

    {
        // Bunny Vehicle!!!!!!!!!!!!!!!!!!!!!!!
        // Who cares about brick?
        Entity& entity = scene.AddEntity();

        auto& transform = entity.AddComponent<Transform>();
        transform.SetPosition(vec3(0.0, 5.0f, 10.0f));
        transform.SetScale(vec3(10.0f, 10.0f, 10.0f));

        auto bunny_vehicle = entity.AddComponent<VehicleComponent>();
        bunny_vehicle.SetVehicleName("BunnyVehicle");

        auto& mesh_renderer = entity.AddComponent<MeshRenderer>();
        mesh_renderer.SetMesh("bunny");
    }
}
