#include "engine/core/math/Physx.h"

using glm::quat;
using glm::vec3;

physx::PxTransform CreateTransform(const vec3& position,
                                   const quat& orientation)
{
    physx::PxVec3 px_position = GlmVecToPxVec(position);
    physx::PxQuat px_orientation(orientation.x, orientation.y, orientation.z,
                                 orientation.w);

    return physx::PxTransform(px_position, px_orientation);
}

physx::PxVec3 GlmVecToPxVec(const vec3& position)
{
    return physx::PxVec3(position.x, position.y, position.z);
}
