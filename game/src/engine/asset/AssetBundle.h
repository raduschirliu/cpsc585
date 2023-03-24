#pragma once

#include <rapidjson/fwd.h>

#include <string>
#include <vector>

struct CubemapRecord
{
    std::string name;
    std::string path_xneg, path_xpos, path_yneg, path_ypos, path_zneg,
        path_zpos;

    bool Deserialize(const rapidjson::Value& node);
};

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
    std::vector<CubemapRecord> cubemaps;

    bool Deserialize(const rapidjson::Value& node);
};
