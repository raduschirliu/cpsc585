#include "engine/render/SceneRenderData.h"

SceneRenderData::SceneRenderData()
    : screen_size(1, 1),
      cameras{},
      entities{},
      point_lights{},
      asset_service(nullptr),
      total_time(0)
{
}