#pragma once

#include <glm/glm.hpp>

#include "PxPhysicsAPI.h"

/**
 * on a successful raycast contains information on hit targets
 */
struct RaycastData
{
    RaycastData(physx::PxRaycastBuffer raycast_result);
    glm::vec3 position;
    glm::vec3 normal;
    float distance;
    physx::PxRigidActor* actor;
};