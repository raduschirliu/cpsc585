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
    ProcessNode(path, name, scene->mRootNode, scene);
}

const Mesh &AssetService::GetMesh(const std::string &name)
{
    ASSERT_MSG(meshes_.find(name) != meshes_.end(),
               "Mesh with given name must exist");
    return meshes_[name];
}

/*
 * .  : Directly access the members of the class
 * -> : Approach the member through a pointer
 */
void AssetService::ProcessNode(const string &path, const string &name,
                               aiNode *node, const aiScene *scene)
{
    for (uint32_t i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes_[name] = ProcessMesh(node, mesh, scene);
    }

    // Do the same for each of its children
    for (uint32_t i = 0; i < node->mNumChildren; ++i)
    {
        ProcessNode(path, name, node->mChildren[i], scene);
    }
}

/*
 * push_back()      : No internal constructor (Create a temporary object from
 *                    the outside)
 * emplace_back()   : Create its own objects internally using the
 *                    constructor
 */
Mesh AssetService::ProcessMesh(aiNode *node, aiMesh *mesh, const aiScene *scene)
{
    Mesh localMesh;
    vector<Texture> textures;

    // Vertex information
    for (uint32_t i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex vertex(glm::vec3(0.f));
        glm::vec3 vector;

        // Position
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;

        // Normal
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;
        }

        // TextureCoord: Maximum 8 texture
        if (mesh->mTextureCoords[0])
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.uv = vec;
        }

        // Tangent
        // vector.x = mesh->mTangents[i].x;
        // vector.y = mesh->mTangents[i].y;
        // vector.z = mesh->mTangents[i].z;
        // vertex.tangent = vector;

        // BiTangent
        // vector.x = mesh->mBitangents[i].x;
        // vector.y = mesh->mBitangents[i].y;
        // vector.z = mesh->mBitangents[i].z;
        // vertex.bitangent = vector;

        localMesh.vertices.emplace_back(vertex);
    }

    // Index information: May vary depending on the value of the Winding flag
    for (uint32_t i = 0; i < mesh->mNumFaces; ++i)
    {
        const aiFace &face = mesh->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; ++j)
        {
            localMesh.indices.emplace_back(face.mIndices[j]);
        }
    }

    // Texture (Material) information
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

    return localMesh;
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

    TestLoadMesh("resources/models/track/track3-9.gltf");
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