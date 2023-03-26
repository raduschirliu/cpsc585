#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "engine/fwd/FwdComponents.h"
#include "engine/fwd/FwdServices.h"

struct SceneRenderData
{
    glm::ivec2 screen_size;
    std::vector<Camera*> cameras;
    std::vector<const Entity*> entities;
    std::vector<PointLight*> point_lights;
    AssetService* asset_service;

    SceneRenderData();
};
