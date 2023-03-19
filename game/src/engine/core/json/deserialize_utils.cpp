#include "engine/core/json/deserialize_utils.h"

#include <rapidjson/document.h>

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

}  // namespace json