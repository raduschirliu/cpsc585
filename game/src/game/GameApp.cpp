#include "game/GameApp.h"

#include <yaml-cpp/yaml.h>

#include <assimp/Importer.hpp>
#include <string>

#include "engine/asset/AssetService.h"
#include "engine/core/debug/Assert.h"
#include "engine/core/debug/Log.h"
#include "engine/config/ConfigService.h"
#include "engine/gui/GuiService.h"
#include "engine/input/InputService.h"
#include "engine/physics/CubeRigidbody.h"
#include "engine/physics/PhysicsService.h"
#include "engine/physics/PlaneRigidbody.h"
#include "engine/physics/SphereRigidbody.h"
#include "engine/render/Camera.h"
#include "engine/render/MeshRenderer.h"
#include "engine/render/RenderService.h"
#include "engine/scene/ComponentUpdateService.h"
#include "engine/scene/Scene.h"
#include "engine/scene/Transform.h"
#include "game/components/BasicComponent.h"
#include "game/components/DebugCameraController.h"
#include "game/components/GuiExampleComponent.h"

using glm::ivec2;
using glm::vec3;
using std::make_unique;
using std::string;

using namespace std;

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

    // Model importing test
    Assimp::Importer importer;
    const aiScene* cube_scene =
        importer.ReadFile("resources/models/cube.obj", 0);
    ASSERT_MSG(cube_scene, "Import must be succesful");
}

/**
 * Runs after all services have been initialized - it's safe to interact with
 * them here if needed
 */
void GameApp::OnStart()
{
    Scene& scene = AddScene("TestScene");

    {
        Entity& entity = scene.AddEntity();

        auto& transform = entity.AddComponent<Transform>();
        transform.SetPosition(vec3(10.0f, 0.0f, 0.0f));

        entity.AddComponent<GuiExampleComponent>();
    }

    {
        // Camera
        Entity& camera = scene.AddEntity();
        Transform& camera_transform = camera.AddComponent<Transform>();
        camera_transform.SetPosition(vec3(0.0f, 10.0f, 15.0f));
        camera.AddComponent<Camera>();
        camera.AddComponent<DebugCameraController>();
    }

    {
        // Floor
        Entity& floor = scene.AddEntity();

        Transform& transform = floor.AddComponent<Transform>();
        transform.SetPosition(vec3(0, 0, 0));

        floor.AddComponent<PlaneRigidbody>();

        auto& mesh_renderer = floor.AddComponent<MeshRenderer>();
        mesh_renderer.SetMesh("plane");
    }

    {
        // Cube
        Entity& entity = scene.AddEntity();

        Transform& transform = entity.AddComponent<Transform>();
        transform.SetPosition(vec3(0.0, 5.0f, 0.0f));

        auto& rigidbody = entity.AddComponent<CubeRigidbody>();
        rigidbody.CreateCube(5.0f, 5.0f, 5.0f);
        rigidbody.SetCanControl(true);

        auto& mesh_renderer = entity.AddComponent<MeshRenderer>();
        mesh_renderer.SetMesh("cube");
    }

    {
        // Bunny
        Entity& entity = scene.AddEntity();

        auto& transform = entity.AddComponent<Transform>();
        transform.SetPosition(vec3(0.0, 5.0f, 10.0f));
        transform.SetScale(vec3(50.0f, 50.0f, 50.0f));

        auto& mesh_renderer = entity.AddComponent<MeshRenderer>();
        mesh_renderer.SetMesh("bunny");
    }
}
