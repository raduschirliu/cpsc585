#pragma once

#include <cstdint>
#include <glm/glm.hpp>

using Color3u = glm::u8vec3;
using Color4u = glm::u8vec4;
using Color3 = glm::vec3;
using Color4 = glm::vec4;

namespace colors
{

constexpr glm::vec3 kRed(1.0f, 0.0f, 0.0f);
constexpr glm::vec3 kGreen(0.0f, 1.0f, 0.0f);
constexpr glm::vec3 kBlue(0.0f, 0.0f, 1.0f);
constexpr glm::vec3 kCyan(0.0f, 1.0f, 1.0f);
constexpr glm::vec3 kMagenta(1.0f, 0.0f, 1.0f);
constexpr glm::vec3 kYellow(1.0f, 1.0f, 0.0f);
constexpr glm::vec3 kOffWhite(0.9f, 0.9f, 0.9f);
constexpr glm::vec3 kWhite(1.0f, 1.0f, 1.0f);

}  // namespace colors
