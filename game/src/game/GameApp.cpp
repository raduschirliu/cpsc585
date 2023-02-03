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

    // Entity& entity1 = scene.AddEntity();
    // Transform& entity1_transform = entity1.AddComponent<Transform>();
    //// setting the transformation of the entity to this, and connecting
    /// physics / in BasicComponent.cpp
    // entity1_transform.SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    // entity1.AddComponent<SphereRigidbody>();
    // entity1.AddComponent<MeshRenderer>();

    Entity& entity2 = scene.AddEntity();
    Transform& entity2_transform = entity2.AddComponent<Transform>();
    entity2_transform.SetPosition(glm::vec3(10.0f, 0.0f, 0.0f));
    entity2.AddComponent<GuiExampleComponent>();

    Entity& camera = scene.AddEntity();
    Transform& camera_transform = camera.AddComponent<Transform>();
    camera_transform.SetPosition(glm::vec3(0.0f, 10.0f, 15.0f));
    camera.AddComponent<Camera>();
    camera.AddComponent<DebugCameraController>();

    /*Entity& cube = scene.AddEntity();
    Transform& cube_transform = cube.AddComponent<Transform>();
    cube_transform.SetPosition(glm::vec3(0.0f, 50.0f, 0.0f));
    auto& cube_sphere = cube.AddComponent<SphereRigidbody>();
    cube_sphere.SetRadius(3.f);
    cube.AddComponent<MeshRenderer>();*/

    Entity& floor = scene.AddEntity();
    Transform& t = floor.AddComponent<Transform>();
    t.SetPosition(glm::vec3(0, 0, 0));
    floor.AddComponent<PlaneRigidbody>();
    floor.AddComponent<MeshRenderer>();

    Entity& cubeRigidBody = scene.AddEntity();
    Transform& temp_cube_transform = cubeRigidBody.AddComponent<Transform>();
    temp_cube_transform.SetPosition(glm::vec3(0.0, 5.f, 0.f));
    auto& rigidBody_ref = cubeRigidBody.AddComponent<CubeRigidbody>();
    rigidBody_ref.CreateCube(5.f, 5.f, 5.f);
    rigidBody_ref.SetCanControl(true);
    cubeRigidBody.AddComponent<MeshRenderer>();
    SetActiveScene("TestScene");

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
        // mesh_renderer.SetMesh("plane");
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
        // Cube 2
        Entity& entity = scene.AddEntity();

        Transform& transform = entity.AddComponent<Transform>();
        transform.SetPosition(vec3(0.0, 5.0f, 0.0f));

        auto& rigidbody = entity.AddComponent<CubeRigidbody>();
        rigidbody.CreateCube(5.0f, 5.0f, 5.0f);
        rigidbody.SetCanControl(false);

        auto& mesh_renderer = entity.AddComponent<MeshRenderer>();
        mesh_renderer.SetMesh("cube");
    }

    {
        // Bunny
        Entity& entity = scene.AddEntity();

        auto& transform = entity.AddComponent<Transform>();
        transform.SetPosition(vec3(0.0, 5.0f, 10.0f));
        transform.SetScale(vec3(40.0f, 40.0f, 40.0f));

        auto& mesh_renderer = entity.AddComponent<MeshRenderer>();
        mesh_renderer.SetMesh("bunny");
    }
}
