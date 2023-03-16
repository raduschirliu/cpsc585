#include "engine/asset/AssetService.h"

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

#include <assimp/Importer.hpp>
#include <filesystem>
#include <fstream>

#include "engine/asset/AssetBundle.h"
#include "engine/core/debug/Log.h"
#include "engine/scene/Entity.h"

using glm::vec3;
using rapidjson::Document;
using rapidjson::IStreamWrapper;
using rapidjson::SizeType;
using std::make_unique;
using std::string;
using std::string_view;
using std::vector;

static const string kAssetFilePath = "resources/assets.jsonc";

void AssetService::LoadMesh(const string &path, const string &name)
{
    Assimp::Importer importer;

    unsigned int flags = aiProcessPreset_TargetRealtime_Fast;

    const aiScene *scene = importer.ReadFile(path, flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->mRootNode)
    {
        Log::error("Failed to import: {}", importer.GetErrorString());
        ASSERT_MSG(false, "Import must be successful");
    }

    ASSERT_MSG(scene->mNumMeshes > 0, "Must have at least 1 mesh");

    if (scene->mNumMeshes == 1)
    {
        ProcessMesh(scene->mMeshes[0], name);
    }
    else
    {
        for (uint32_t i = 0; i < scene->mNumMeshes; i++)
        {
            aiMesh *mesh = scene->mMeshes[i];
            const string mesh_name =
                fmt::format("{}@{}", name, mesh->mName.C_Str());
            ProcessMesh(mesh, mesh_name);
        }
    }
}

const Mesh &AssetService::GetMesh(const std::string &name)
{
    ASSERT_MSG(meshes_.find(name) != meshes_.end(),
               "Mesh with given name must exist");
    return *meshes_[name];
}

void AssetService::ProcessMesh(aiMesh *mesh, const string &name)
{
    auto processed_mesh = make_unique<Mesh>();

    // Vertex information
    for (uint32_t i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex(vec3(0.0f, 0.0f, 0.0f));

        // Position
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;

        // Normal
        if (mesh->HasNormals())
        {
            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;
        }

        // Texture UVs
        if (mesh->HasTextureCoords(0))
        {
            vertex.uv.x = mesh->mTextureCoords[0][i].x;
            vertex.uv.y = mesh->mTextureCoords[0][i].y;
        }

        processed_mesh->vertices.emplace_back(vertex);
    }

    // Index information: May vary depending on the value of the Winding flag
    for (uint32_t i = 0; i < mesh->mNumFaces; i++)
    {
        const aiFace &face = mesh->mFaces[i];
        processed_mesh->indices.insert(processed_mesh->indices.end(),
                                       face.mIndices,
                                       face.mIndices + face.mNumIndices);
    }

    meshes_[name] = std::move(processed_mesh);
}

void AssetService::LoadTexture(const string &path, const string &name)
{
    ASSERT_MSG(textures_.find(name) == textures_.end(),
               "Textures must have unique names");

    textures_[name] = make_unique<Texture>(path);
}

const Texture &AssetService::GetTexture(const string &name)
{
    auto iter = textures_.find(name);
    ASSERT_MSG(iter != textures_.end(), "Texture with given name must exist");
    return *iter->second;
}

void AssetService::OnInit()
{
    LoadAssetFile(kAssetFilePath);

    Log::info("Loaded meshes: {}", meshes_.size());
    Log::info("Loaded textures: {}", textures_.size());
}

void AssetService::OnStart(ServiceProvider &service_provider)
{
}

void AssetService::OnUpdate()
{
}

void AssetService::OnCleanup()
{
}

string_view AssetService::GetName() const
{
    return "AssetService";
}

void AssetService::TestLoadMesh(const string &path)
{
    Assimp::Importer importer;
    const uint32_t flags = aiProcessPreset_TargetRealtime_Fast;

    const aiScene *scene = importer.ReadFile(path, flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->mRootNode)
    {
        Log::error("Failed to import: {}", importer.GetErrorString());
        ASSERT_MSG(false, "Import must be successful");
    }

    Log::info("Scene: {}", scene->mName.C_Str());
    for (size_t i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[i];

        Log::info("\t- Mesh: {}", mesh->mName.C_Str());
    }
}

void AssetService::LoadAssetFile(const string &path)
{
    std::ifstream file_stream(path);
    ASSERT_MSG(file_stream.is_open(), "Failed to open asset file");

    IStreamWrapper stream(file_stream);
    Document doc;
    doc.ParseStream(stream);
    ASSERT_MSG(doc.IsObject(), "Asset file must be an object");

    AssetBundle bundle;
    const bool deserialize_status = bundle.Deserialize(doc);
    ASSERT_MSG(deserialize_status, "Must be valid AssetBundle");

    for (auto &mesh : bundle.meshes)
    {
        LoadMesh(mesh.path, mesh.name);
    }

    for (auto &texture : bundle.textures)
    {
        LoadTexture(texture.path, texture.name);
    }
}