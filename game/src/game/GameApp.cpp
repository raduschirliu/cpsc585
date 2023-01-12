#include "game/GameApp.h"

#include <yaml-cpp/yaml.h>

#include <assimp/Importer.hpp>
#include <string>

#include "engine/core/debug/Assert.h"
#include "engine/core/debug/Log.h"
#include "engine/physics/PhysicsService.h"
#include "engine/render/RenderService.h"
#include "engine/scene/Scene.h"
#include "game/components/BasicComponent.h"
#include "game/components/ComplexComponent.h"

using glm::ivec2;
using std::string;

GameApp::GameApp()
{
}

void GameApp::Init()
{
    GetWindow().SetSize(ivec2(1280, 720));

    AddService<PhysicsService>();
    AddService<RenderService>();

    Scene& scene = AddScene("TestScene");
    Entity& entity = scene.AddEntity();
    entity.AddComponent<BasicComponent>();
    entity.AddComponent<ComplexComponent>();

    // Model importing test
    Assimp::Importer importer;
    const aiScene* cube_scene =
        importer.ReadFile("resources/models/cube.obj", 0);
    ASSERT_MSG(cube_scene, "Import must be succesful");

    // Yaml parsing test
    YAML::Node root = YAML::LoadFile("resources/scenes/test.yaml");
    Log::debug("someRootNode.someChildNode = {}",
               root["someRootNode"]["someChildNode"].as<string>());
}