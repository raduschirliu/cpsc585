#include "engine/render/SceneRenderData.h"

SceneRenderData::SceneRenderData()
    : screen_size(1.0f),
      cameras{},
      entities{},
      point_lights{},
      asset_service(nullptr)
{
}