#include "engine/asset/AssetBundle.h"

#include <rapidjson/document.h>

#include "engine/core/json/deserialize_utils.h"

using rapidjson::Document;
using rapidjson::Value;
using std::optional;
using std::string;
using std::vector;

template <class T>
static void ReadRecordArray(vector<T>& res, const Value& parent_node,
                            const string& name);

template <class T>
void ReadRecordArray(vector<T>& res, const Value& parent_node,
                     const string& name)
{
    Value::ConstMemberIterator iter = parent_node.FindMember(name);

    if (iter == parent_node.MemberEnd())
    {
        return;
    }

    for (const Value& obj : iter->value.GetArray())
    {
        T record;
        record.Deserialize(obj);
        res.push_back(record);
    }
}

bool CubemapRecord::Deserialize(const Value& node)
{
    if (!node.IsObject())
    {
        return false;
    }

    bool status = true;
    status &= json::GetString(node, "name", name);
    status &= json::GetString(node, "path_xneg", path_xneg);
    status &= json::GetString(node, "path_xpos", path_xpos);
    status &= json::GetString(node, "path_yneg", path_yneg);
    status &= json::GetString(node, "path_ypos", path_ypos);
    status &= json::GetString(node, "path_zneg", path_zneg);
    status &= json::GetString(node, "path_zpos", path_zpos);

    return status;
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

    ReadRecordArray(meshes, node, "meshes");
    ReadRecordArray(textures, node, "textures");
    ReadRecordArray(cubemaps, node, "cubemaps");

    return true;
}
