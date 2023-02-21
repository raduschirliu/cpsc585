#pragma once

#include <imgui.h>

#include <glm/glm.hpp>
#include <string>

namespace gui
{

void ViewProperty(const std::string& label, const glm::vec3& value);

bool EditProperty(const std::string& label, glm::vec3& value);

void EditColorProperty(const std::string& label, glm::vec3& value);

};  // namespace gui
