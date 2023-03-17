#pragma once

#include <rapidjson/fwd.h>

#include <string>
#include <vector>

struct AssetRecord
{
    std::string path;
    std::string name;

    bool Deserialize(const rapidjson::Value& node);
};

struct AssetBundle
{
    std::vector<AssetRecord> meshes;
    std::vector<AssetRecord> textures;

    bool Deserialize(const rapidjson::Value& node);
};
