#include "engine/core/math/Physx.h"

using glm::quat;
using glm::vec3;
using glm::vec4;
using physx::PxQuat;
using physx::PxTransform;
using physx::PxVec3;

struct PxColorRepresentation
{
    uint8_t blue;  // Lowest address
    uint8_t green;
    uint8_t red;
    uint8_t alpha;  // Highest address
};

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
    return quat(px_quat.w, px_quat.x, px_quat.y, px_quat.z);
}

Color4u PxColorToVec(uint32_t color)
{
    PxColorRepresentation* data =
        reinterpret_cast<PxColorRepresentation*>(&color);

    return Color4u(data->red, data->green, data->blue, data->alpha);
}