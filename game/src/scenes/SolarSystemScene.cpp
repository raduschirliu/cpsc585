#include "scenes/SolarSystemScene.h"

#include <GLFW/glfw3.h>
#include <imgui.h>

#include "core/Input.h"
#include "core/gfx/Log.h"
#include "core/material/EarthMaterial.h"
#include "core/material/PhongMaterial.h"
#include "core/material/TextureMaterial.h"

using glm::vec3;
using std::make_shared;
using std::make_unique;
using std::shared_ptr;
using std::string;

static constexpr float kMaxTimeScale = 20.0f;

static constexpr float RevolutionPeriodToSpeed(float period_days);
static constexpr float RotationPeriodToSpeed(float period_hrs);
static constexpr float ScaleOrbitRadius(float distance_mill_km);
static constexpr float ScaleOrbitalInclination(float inclination_deg);

static constexpr float RevolutionPeriodToSpeed(float period_days)
{
    constexpr float speedup_factor = 500000.0f;
    float period_sec = period_days * 24.0f * 3600.0f;
    return glm::two_pi<float>() / period_sec * speedup_factor;
}

static constexpr float RotationPeriodToSpeed(float period_hrs)
{
    constexpr float speedup_factor = 10000.0f;
    float period_sec = period_hrs * 3600.0f;
    return glm::two_pi<float>() / period_sec * speedup_factor;
}

static constexpr float ScaleOrbitRadius(float distance_mill_km)
{
    return distance_mill_km * 1.0f;
}

static constexpr float ScaleOrbitalInclination(float inclination_deg)
{
    return glm::radians(inclination_deg) * 5.0f;
}

SolarSystemScene::SolarSystemScene()
    : Scene(make_unique<OrbitCamera3d>(90.0f, 400.0f)),
      camera_(Camera<OrbitCamera3d>()),
      show_origin_marker_(false),
      show_axis_markers_(false),
      show_normals_(false),
      wireframe_(false),
      origin_marker_obj_(nullptr),
      planets_{},
      normal_markers_{}
{
    // Build origin marker
    origin_marker_obj_ = make_shared<OriginMarkerObject>();
    AddGameObject(origin_marker_obj_);

    // Build planets
    SetupPlanets();

    UpdatePlanetDebugDraws();
}

void SolarSystemScene::Init()
{
    Scene::Init();
}

void SolarSystemScene::RenderGui()
{
    Scene::RenderGui();

    bool debug_draws_dirty = false;
    debug_draws_dirty |= ImGui::Checkbox("Origin marker", &show_origin_marker_);
    debug_draws_dirty |=
        ImGui::Checkbox("Show axis markers", &show_axis_markers_);
    debug_draws_dirty |= ImGui::Checkbox("Show normals", &show_normals_);
    debug_draws_dirty |= ImGui::Checkbox("Wireframe", &wireframe_);

    if (debug_draws_dirty)
    {
        UpdatePlanetDebugDraws();
    }

    ImGui::SliderFloat("Time scale", &utils::kTimeScale, 0.0f, kMaxTimeScale);

    if (ImGui::Button("Reset animation"))
    {
        ResetPlanets();
    }
}

void SolarSystemScene::UpdatePlanetDebugDraws()
{
    origin_marker_obj_->SetActive(show_origin_marker_);

    for (auto& planet : planets_)
    {
        planet->SetWireframe(wireframe_);
        planet->SetAxisMarkerVisible(show_axis_markers_);
    }

    for (auto& obj : normal_markers_)
    {
        obj->SetActive(show_normals_);
    }
}

void SolarSystemScene::ResetPlanets()
{
    for (auto& planet : planets_)
    {
        planet->Reset();
    }
}

shared_ptr<PlanetObject> SolarSystemScene::NewPlanet(float diameter_km)
{
    // Scale down
    float radius = diameter_km / 2.0f;

    auto planet = make_shared<PlanetObject>(radius);
    ASSERT_MSG(planet, "New planet must be valid");

    planets_.push_back(planet);
    AddGameObject(planet);

    auto normal_marker = make_shared<NormalMarkerObject>(*planet);
    normal_marker->SetActive(show_normals_);
    normal_markers_.push_back(normal_marker);
    planet->AddChild(normal_marker);

    return planet;
}

shared_ptr<Texture> SolarSystemScene::NewTexture(std::string path)
{
    return make_shared<Texture>(path, Texture::InterpolationMode::kLinear);
}

void SolarSystemScene::SetupPlanets()
{
    // Textures
    auto stars_texture = NewTexture("textures/stars.jpg");

    auto earth_diffuse_texture = NewTexture("textures/earth_diffuse_day.jpg");
    auto earth_diffuse_night_texture =
        NewTexture("textures/earth_diffuse_night.jpg");
    auto earth_cloud_texture = NewTexture("textures/earth_clouds.jpg");
    auto earth_specular_texture = NewTexture("textures/earth_specular.jpg");

    auto sun_texture = NewTexture("textures/sun_diffuse.jpg");
    auto mercury_texture = NewTexture("textures/mercury_diffuse.jpg");
    auto moon_texture = NewTexture("textures/moon_diffuse.jpg");
    auto mars_texture = NewTexture("textures/mars_diffuse.jpg");
    auto uranus_texture = NewTexture("textures/uranus_diffuse.jpg");
    auto neptune_texture = NewTexture("textures/neptune_diffuse.jpg");
    auto venus_surface_texture =
        NewTexture("textures/venus_surface_diffuse.jpg");

    // Build planets
    auto skybox = make_shared<PlanetObject>(10000.0f);
    skybox->SetAxisMarkerVisible(false);
    skybox->SetMaterial(make_shared<TextureMaterial>(stars_texture));
    AddGameObject(skybox);

    auto sun = NewPlanet(200.0f);
    sun->SetAxialRotation(glm::radians(7.25f), RotationPeriodToSpeed(609.0f));
    sun->SetMaterial(make_shared<TextureMaterial>(sun_texture));

    auto mercury = NewPlanet(10.0f);
    mercury->SetAxialRotation(glm::radians(0.0f),
                              RotationPeriodToSpeed(1000.0f));
    mercury->SetOrbitalRevolution(sun, ScaleOrbitRadius(115.0f),
                                  ScaleOrbitalInclination(7.0f),
                                  RevolutionPeriodToSpeed(88.0f));
    mercury->SetMaterial(make_shared<PhongMaterial>(
        mercury_texture, vec3(0.5f, 0.5f, 0.5f), 128.0f));

    auto earth = NewPlanet(20.0f);
    earth->SetAxialRotation(glm::radians(23.44f), RotationPeriodToSpeed(24.0f));
    // Distance should be ~150.0f, orbital incl should be ~7.15f
    earth->SetOrbitalRevolution(sun, ScaleOrbitRadius(150.0f),
                                ScaleOrbitalInclination(25.0f),
                                RevolutionPeriodToSpeed(365.0f));
    earth->SetMaterial(make_shared<EarthMaterial>(
        earth_diffuse_texture, earth_diffuse_night_texture, earth_cloud_texture,
        earth_specular_texture, 64.0f));

    auto moon = NewPlanet(5.0f);
    moon->SetAxialRotation(glm::radians(6.7f), RotationPeriodToSpeed(655.0f));
    moon->SetOrbitalRevolution(earth, ScaleOrbitRadius(10.0f),
                               ScaleOrbitalInclination(0.0f),
                               RevolutionPeriodToSpeed(27.3f));
    moon->SetMaterial(make_shared<PhongMaterial>(
        moon_texture, vec3(0.5f, 0.5f, 0.5f), 64.0f));

    auto mars = NewPlanet(10.0f);
    mars->SetAxialRotation(glm::radians(25.2f), RotationPeriodToSpeed(24.6f));
    mars->SetOrbitalRevolution(sun, ScaleOrbitRadius(230.0f),
                               ScaleOrbitalInclination(5.60f),
                               RevolutionPeriodToSpeed(687.0f));
    mars->SetMaterial(make_shared<PhongMaterial>(
        mars_texture, vec3(0.5f, 0.5f, 0.5f), 64.0f));

    auto uranus = NewPlanet(100.0f);
    uranus->SetAxialRotation(glm::radians(97.8f),
                             RotationPeriodToSpeed(-17.2f));
    uranus->SetOrbitalRevolution(sun, ScaleOrbitRadius(500.0f),
                                 ScaleOrbitalInclination(7.0f),
                                 RevolutionPeriodToSpeed(30000.0f));
    uranus->SetMaterial(make_shared<PhongMaterial>(
        uranus_texture, vec3(0.5f, 0.5f, 0.5f), 16.0f));
}