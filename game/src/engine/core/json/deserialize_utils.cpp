#include "engine/core/json/deserialize_utils.h"

#include <rapidjson/document.h>

#include <glm/glm.hpp>

using glm::vec3;
using rapidjson::Document;
using rapidjson::Value;
using std::nullopt;
using std::optional;
using std::string;

namespace json
{

bool GetString(const Value& node, const string& name, string& out_val)
{
    Value::ConstMemberIterator iter = node.FindMember(name);

    if (iter == node.MemberEnd())
    {
        return false;
    }

    if (!iter->value.IsString())
    {
        return false;
    }

    out_val = iter->value.GetString();
    return true;
}

bool GetVec3(const Value& node, const string& name, vec3& out_val)
{
    Value::ConstMemberIterator iter = node.FindMember(name);

    if (iter == node.MemberEnd())
    {
        return false;
    }

    if (!iter->value.IsArray())
    {
        return false;
    }

    auto arr = iter->value.GetArray();

    if (arr.Size() != 3)
    {
        return false;
    }

    if (!arr[0].IsFloat() || !arr[1].IsFloat() || !arr[2].IsFloat())
    {
        return false;
    }

    out_val.x = arr[0].GetFloat();
    out_val.y = arr[1].GetFloat();
    out_val.z = arr[2].GetFloat();

    return true;
}

}  // namespace json