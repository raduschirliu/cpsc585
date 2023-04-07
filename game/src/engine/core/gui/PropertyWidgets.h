#pragma once

#include <imgui.h>

#include <glm/glm.hpp>
#include <string>

#include "engine/core/math/Rect2d.h"

namespace gui
{

void ViewProperty(const std::string& label, const glm::vec3& value);

bool EditProperty(const std::string& label, glm::vec3& value);
bool EditProperty(const std::string& label, glm::vec2& value);
bool EditProperty(const std::string& label, Rect2d& value);

void EditColorProperty(const std::string& label, glm::vec3& value);

};  // namespace gui
