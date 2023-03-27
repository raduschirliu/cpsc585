#pragma once

#include <glm/glm.hpp>

#include "PxPhysicsAPI.h"
#include "engine/scene/Entity.h"

/**
 * on a successful raycast contains information on hit targets
 */
struct RaycastData
{
    RaycastData(physx::PxRaycastBuffer raycast_result, Entity* entity);
    glm::vec3 position;
    glm::vec3 normal;
    float distance;
    Entity* entity;
    physx::PxRigidActor* actor;
};