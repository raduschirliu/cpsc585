#pragma once

#include <rapidjson/fwd.h>

#include <glm/fwd.hpp>
#include <optional>
#include <string>

namespace json
{

bool GetString(const rapidjson::Value& node, const std::string& name,
               std::string& out_val);
bool GetVec3(const rapidjson::Value& node, const std::string& name,
             glm::vec3& out_val);

}  // namespace json