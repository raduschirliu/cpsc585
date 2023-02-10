#pragma once

#include <glm/glm.hpp>

#include "PxPhysicsAPI.h"

struct RaycastData
{
    RaycastData(physx::PxRaycastBuffer raycast_result);
    glm::vec3 position;
    glm::vec3 normal;
    float distance;
    // physx::PxRigidActor* actor;
};