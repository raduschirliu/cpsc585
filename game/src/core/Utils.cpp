#include "Utils.h"

using glm::vec2;
using glm::vec3;

namespace utils
{

static constexpr float kThetaMax = glm::pi<float>();
static constexpr float kPhiMax = glm::two_pi<float>();

float kTimeScale = 1.0f;

vec3 SphericalToCartesian(float radius, float phi, float theta)
{
    // Swap Y & Z coordinates as most math coordinate systems use +Z as
    // the "up" direction instead of +Y (like most games)
    return vec3(radius * glm::sin(theta) * glm::cos(phi),
                radius * glm::cos(theta),
                radius * glm::sin(theta) * glm::sin(phi));
}

vec2 SphericalToUv(float radius, float phi, float theta)
{
    // Flip both coordinates, as (0, 0) is bottom-left corner in texture coordinates
    return vec2(phi / kPhiMax, theta / kThetaMax);
}

}  // namespace utils