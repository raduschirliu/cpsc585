#include "RaycastData.h"

#include <glm/glm.hpp>

#include "PxPhysicsAPI.h"
#include "engine/core/math/Physx.h"

using physx::PxRaycastBuffer;

RaycastData::RaycastData(PxRaycastBuffer raycast_result)
{
    RaycastData::position = glm::vec3(raycast_result.block.position.x,
                                      raycast_result.block.position.y,
                                      raycast_result.block.position.z);

    RaycastData::normal =
        glm::vec3(raycast_result.block.normal.x, raycast_result.block.normal.y,
                  raycast_result.block.normal.z);

    RaycastData::distance = raycast_result.block.distance;

    RaycastData::actor = raycast_result.block.actor;
}