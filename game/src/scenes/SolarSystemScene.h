#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "core/ShaderManager.h"
#include "core/entity/OrbitCamera3d.h"
#include "core/scene/Scene.h"
#include "objects/NormalMarkerObject.h"
#include "objects/OriginMarkerObject.h"
#include "objects/PlanetObject.h"

class SolarSystemScene final : public Scene
{
  public:
    SolarSystemScene();

    // From Scene
    void Init() override;
    void RenderGui() override;

  private:
    OrbitCamera3d& camera_;
    bool show_origin_marker_;
    bool show_axis_markers_;
    bool show_normals_;
    bool wireframe_;
    std::shared_ptr<OriginMarkerObject> origin_marker_obj_;
    std::vector<std::shared_ptr<PlanetObject>> planets_;
    std::vector<std::shared_ptr<NormalMarkerObject>> normal_markers_;

    void UpdatePlanetDebugDraws();
    std::shared_ptr<PlanetObject> NewPlanet(float radius);
    void SetupPlanets();
    void ResetPlanets();
    std::shared_ptr<Texture> NewTexture(std::string path);
};