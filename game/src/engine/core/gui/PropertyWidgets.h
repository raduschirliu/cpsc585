#pragma once

#include <imgui.h>

#include <glm/glm.hpp>
#include <string>

namespace gui
{

void Vec3Field(const std::string& label, glm::vec3& value);

};
