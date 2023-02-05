#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "PxPhysicsAPI.h"

static constexpr glm::quat kIdentityQuat(1.0f, 0.0f, 0.0f, 0.0f);

struct GlmTransform
{
    glm::vec3 position;
    glm::quat orientation;
};

physx::PxTransform CreatePxTransform(const glm::vec3& position,
                                   const glm::quat& orientation);

physx::PxVec3 GlmToPx(const glm::vec3& glm_vec);

GlmTransform PxToGlm(const physx::PxTransform& px_transform);
glm::vec3 PxToGlm(const physx::PxVec3& px_vec);
glm::quat PxToGlm(const physx::PxQuat& px_quat);