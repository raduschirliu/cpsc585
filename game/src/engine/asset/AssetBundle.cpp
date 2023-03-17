#include "engine/asset/AssetBundle.h"

#include <rapidjson/document.h>

#include "engine/core/json/deserialize_utils.h"

using rapidjson::Document;
using rapidjson::Value;
using std::optional;
using std::string;
using std::vector;

static void ReadAssetRecordArray(vector<AssetRecord>& res,
                                 const Value& parent_node, const string& name);

void ReadAssetRecordArray(vector<AssetRecord>& res, const Value& parent_node,
                          const string& name)
{
    Value::ConstMemberIterator iter = parent_node.FindMember(name);

    if (iter == parent_node.MemberEnd())
    {
        return;
    }

    for (const Value& obj : iter->value.GetArray())
    {
        AssetRecord record;
        record.Deserialize(obj);
        res.push_back(record);
    }
}

bool AssetRecord::Deserialize(const Value& node)
{
    if (!node.IsObject())
    {
        return false;
    }

    // .path
    if (!json::GetString(node, "path", path))
    {
        return false;
    }

    // .name
    if (!json::GetString(node, "name", name))
    {
        return false;
    }

    return true;
}

bool AssetBundle::Deserialize(const Value& node)
{
    if (!node.IsObject())
    {
        return false;
    }

    ReadAssetRecordArray(meshes, node, "meshes");
    ReadAssetRecordArray(textures, node, "textures");

    return true;
}
