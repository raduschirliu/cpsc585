#include "engine/core/math/Physx.h"

using glm::quat;
using glm::vec3;
using physx::PxQuat;
using physx::PxTransform;
using physx::PxVec3;

PxTransform CreatePxTransform(const vec3& position, const quat& orientation)
{
    PxVec3 px_position = GlmToPx(position);
    PxQuat px_orientation(orientation.x, orientation.y, orientation.z,
                          orientation.w);

    return PxTransform(px_position, px_orientation);
}

PxVec3 GlmToPx(const vec3& glm_vec)
{
    return PxVec3(glm_vec.x, glm_vec.y, glm_vec.z);
}

GlmTransform PxToGlm(const PxTransform& px_transform)
{
    return GlmTransform{.position = PxToGlm(px_transform.p),
                        .orientation = PxToGlm(px_transform.q)};
}

vec3 PxToGlm(const PxVec3& px_vec)
{
    return vec3(px_vec.x, px_vec.y, px_vec.z);
}

quat PxToGlm(const PxQuat& px_quat)
{
    return quat(px_quat.x, px_quat.y, px_quat.z, px_quat.w);
}