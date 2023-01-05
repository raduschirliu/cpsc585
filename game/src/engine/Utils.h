#pragma once

#include <array>
#include <cassert>
#include <glm/glm.hpp>
#include <glm/gtx/common.hpp>

#include "engine/gfx/Geometry.h"

#define UNUSED(x) (void)(x)

#define ASSERT_MSG(condition, msg) assert((condition)&& msg)

#define TODO(default, msg) \
    assert(false && msg);  \
    return default;

namespace utils
{

constexpr glm::mat4 kIdentityMatrix(1.0f);

extern float kTimeScale;

template <class Type>
Type Lerp(Type start, Type end, float t)
{
    t = glm::clamp(t, 0.0f, 1.0f);

    Type diff = end - start;
    return start + t * diff;
}

// Map x from [a, b] to y in [c, d]
template <class T>
constexpr T Map(T x, T a, T b, T c, T d)
{
    return (x - a) / (b - a) * (d - c) + c;
}

// Convert spherical coordinates for a sphere centered at origin to cartesian
// coordinates
glm::vec3 SphericalToCartesian(float radius, float phi, float theta);

// Convert spherical coordinates to their corresponding UV mappings
glm::vec2 SphericalToUv(float radius, float phi, float theta);

}  // namespace utils

namespace utils::colors
{

constexpr glm::vec3 kRed(1.0f, 0.0f, 0.0f);
constexpr glm::vec3 kGreen(0.0f, 1.0f, 0.0f);
constexpr glm::vec3 kBlue(0.0f, 0.0f, 1.0f);
constexpr glm::vec3 kCyan(0.0f, 1.0f, 1.0f);
constexpr glm::vec3 kMagenta(1.0f, 0.0f, 1.0f);
constexpr glm::vec3 kYellow(1.0f, 1.0f, 0.0f);
constexpr glm::vec3 kOffWhite(0.9f, 0.9f, 0.9f);
constexpr glm::vec3 kWhite(1.0f, 1.0f, 1.0f);

}  // namespace utils::colors
