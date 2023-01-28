#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "PxPhysicsAPI.h"

physx::PxTransform CreateTransform(const glm::vec3& position,
                                   const glm::quat& orientation);

physx::PxVec3 GlmVecToPxVec(const glm::vec3& position);
