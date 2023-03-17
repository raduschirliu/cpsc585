#pragma once

#include <rapidjson/fwd.h>

#include <optional>
#include <string>

namespace json
{

bool GetString(const rapidjson::Value& node, const std::string& name,
               std::string& out_val);

}  // namespace json