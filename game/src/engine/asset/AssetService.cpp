#include "engine/asset/AssetService.h"

#include "engine/core/debug/Log.h"
#include "engine/scene/Entity.h"

using namespace std;

void AssetService::LoadMesh(const string &path, const string &name,
                            const string &textureName)
{
    Assimp::Importer importer;

    unsigned int flags = aiProcess_Triangulate |
                         aiProcess_JoinIdenticalVertices |
                         aiProcess_CalcTangentSpace | aiProcess_GenNormals;

    const aiScene *scene = importer.ReadFile(path, flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->mRootNode)
    {
        ASSERT_MSG(false, importer.GetErrorString());
    }

    ProcessNode(path, name, textureName, scene->mRootNode, scene);
}

const Mesh &AssetService::GetMesh(const std::string &name)
{
    return meshes_[name];
}

/*
 * .  : Directly access the members of the class
 * -> : Approach the member through a pointer
 */
void AssetService::ProcessNode(const string &path, const string &name,
                               const string &textureName, aiNode *node,
                               const aiScene *scene)
{
    for (uint32_t i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes_[name] = ProcessMesh(node, textureName, mesh, scene);
    }

    // Do the same for each of its children
    for (uint32_t i = 0; i < node->mNumChildren; ++i)
    {
        ProcessNode(path, name, textureName, node->mChildren[i], scene);
    }
}

/*
 * push_back()      : No internal constructor (Create a temporary object from
 *                    the outside)
 * emplace_back()   : Create its own objects internally using the
 *                    constructor
 */
Mesh AssetService::ProcessMesh(aiNode *node, const string &textureName,
                               aiMesh *mesh, const aiScene *scene)
{
    Mesh localMesh;

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
        aiFace face = mesh->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; ++j)
        {
            localMesh.indices.emplace_back(face.mIndices[j]);
        }
    }

    // Texture (Material) information
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

    if (textureName.compare("") < 0){
        // Diffuse Texture: combined with the result of the diffuse lighting
        // equation
        if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            aiString path;
            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
            {
                vector<Texture> diffuseMap = LoadTexture(
                    path.C_Str(), textureName, material, aiTextureType_DIFFUSE);
                for (auto& diffuse : diffuseMap){
                    localMesh.textures.push_back(std::move(diffuse));
                }
            }
        }

        // Specular Texture: combined with the result of the specular lighting
        // equation
        if (material->GetTextureCount(aiTextureType_SPECULAR) > 0)
        {
            aiString path;
            if (material->GetTexture(aiTextureType_SPECULAR, 0, &path) ==
                AI_SUCCESS)
            {
                vector<Texture> specularMap = LoadTexture(
                    path.C_Str(), textureName, material, aiTextureType_SPECULAR);
                for (auto& specular : specularMap){
                    localMesh.textures.push_back(std::move(specular));
                }
            }
        }

        // Ambient Texture: combined with the result of the ambient lighting
        // equation
        if (material->GetTextureCount(aiTextureType_AMBIENT) > 0)
        {
            aiString path;
            if (material->GetTexture(aiTextureType_AMBIENT, 0, &path) == AI_SUCCESS)
            {
                vector<Texture> ambientMap = LoadTexture(
                    path.C_Str(), textureName, material, aiTextureType_AMBIENT);
                for (auto& ambient : ambientMap){
                    localMesh.textures.push_back(std::move(ambient));
                }
            }
        }

        // Emissive Texture: added to the result of the lighting calculation (X
        // influenced by incoming light)
        if (material->GetTextureCount(aiTextureType_EMISSIVE) > 0)
        {
            aiString path;
            if (material->GetTexture(aiTextureType_EMISSIVE, 0, &path) ==
                AI_SUCCESS)
            {
                vector<Texture> emissiveMap = LoadTexture(
                    path.C_Str(), textureName, material, aiTextureType_EMISSIVE);
                for (auto& emissive : emissiveMap){
                    localMesh.textures.push_back(std::move(emissive));
                }
            }
        }

        // Height Texture: higher grey-scale values stand for higher elevations
        // from the base height
        if (material->GetTextureCount(aiTextureType_HEIGHT) > 0)
        {
            aiString path;
            if (material->GetTexture(aiTextureType_HEIGHT, 0, &path) == AI_SUCCESS)
            {
                vector<Texture> heightMap = LoadTexture(
                    path.C_Str(), textureName, material, aiTextureType_HEIGHT);
                for (auto& height : heightMap){
                    localMesh.textures.push_back(std::move(height));
                }
            }
        }

        // Normal Texture: a (tangent space) normal-map
        if (material->GetTextureCount(aiTextureType_NORMALS) > 0)
        {
            aiString path;
            if (material->GetTexture(aiTextureType_NORMALS, 0, &path) == AI_SUCCESS)
            {
                vector<Texture> normalsMap = LoadTexture(
                    path.C_Str(), textureName, material, aiTextureType_NORMALS);
                for (auto& normals : normalsMap){
                    localMesh.textures.push_back(std::move(normals));
                }
            }
        }

        // Shininess Texture: defines the glossiness of the material
        if (material->GetTextureCount(aiTextureType_SHININESS) > 0)
        {
            aiString path;
            if (material->GetTexture(aiTextureType_SHININESS, 0, &path) ==
                AI_SUCCESS)
            {
                vector<Texture> shininessMap = LoadTexture(
                    path.C_Str(), textureName, material, aiTextureType_SHININESS);
                for (auto& shininess : shininessMap){
                    localMesh.textures.push_back(std::move(shininess));
                }
            }
        }

        // Opacity Texture (Transparent): defines per-pixel opacity (white ==
        // opaque and black == transparent)
        if (material->GetTextureCount(aiTextureType_OPACITY) > 0)
        {
            aiString path;
            if (material->GetTexture(aiTextureType_OPACITY, 0, &path) == AI_SUCCESS)
            {
                vector<Texture> opacityMap = LoadTexture(
                    path.C_Str(), textureName, material, aiTextureType_OPACITY);
                for (auto& opacity : opacityMap){
                    localMesh.textures.push_back(std::move(opacity));
                }
            }
        }

        // Displacement Texture: higher color values stand for higher vertex
        // displacement
        if (material->GetTextureCount(aiTextureType_DISPLACEMENT) > 0)
        {
            aiString path;
            if (material->GetTexture(aiTextureType_DISPLACEMENT, 0, &path) ==
                AI_SUCCESS)
            {
                vector<Texture> displacementMap =
                    LoadTexture(path.C_Str(), textureName, material,
                                aiTextureType_DISPLACEMENT);
                for (auto& displacement : displacementMap){
                    localMesh.textures.push_back(std::move(displacement));
                }
            }
        }

        // LightMap Texture: cover both lightmaps and dedicated ambient occlusion
        // maps contains a scaling value for the final color value of a pixel
        if (material->GetTextureCount(aiTextureType_LIGHTMAP) > 0)
        {
            aiString path;
            if (material->GetTexture(aiTextureType_LIGHTMAP, 0, &path) ==
                AI_SUCCESS)
            {
                vector<Texture> lightMap = LoadTexture(
                    path.C_Str(), textureName, material, aiTextureType_LIGHTMAP);
                for (auto& light : lightMap){
                    localMesh.textures.push_back(std::move(light));
                }
            }
        }

        // BaseColor Texture
        if (material->GetTextureCount(aiTextureType_BASE_COLOR) > 0)
        {
            aiString path;
            if (material->GetTexture(aiTextureType_BASE_COLOR, 0, &path) ==
                AI_SUCCESS)
            {
                vector<Texture> baseColorMap = LoadTexture(
                    path.C_Str(), textureName, material, aiTextureType_BASE_COLOR);
                for (auto& baseColor : baseColorMap){
                    localMesh.textures.push_back(std::move(baseColor));
                }
            }
        }

        // Emissive Color Texture
        if (material->GetTextureCount(aiTextureType_EMISSION_COLOR) > 0)
        {
            aiString path;
            if (material->GetTexture(aiTextureType_EMISSION_COLOR, 0, &path) ==
                AI_SUCCESS)
            {
                vector<Texture> emmissionColorMap =
                    LoadTexture(path.C_Str(), textureName, material,
                                aiTextureType_EMISSION_COLOR);
                for (auto& emmissionColor : emmissionColorMap){
                    localMesh.textures.push_back(std::move(emmissionColor));
                }
            }
        }

        // Metalness Texture
        if (material->GetTextureCount(aiTextureType_METALNESS) > 0)
        {
            aiString path;
            if (material->GetTexture(aiTextureType_METALNESS, 0, &path) ==
                AI_SUCCESS)
            {
                vector<Texture> metalnessMap = LoadTexture(
                    path.C_Str(), textureName, material, aiTextureType_METALNESS);
                for (auto& metalness : metalnessMap){
                    localMesh.textures.push_back(std::move(metalness));
                }
            }
        }

        // Roughness Texture
        if (material->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS) > 0)
        {
            aiString path;
            if (material->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &path) ==
                AI_SUCCESS)
            {
                vector<Texture> diffuseRoughnessMap =
                    LoadTexture(path.C_Str(), textureName, material,
                                aiTextureType_DIFFUSE_ROUGHNESS);
                for (auto& diffuseRoughness : diffuseRoughnessMap){
                    localMesh.textures.push_back(std::move(diffuseRoughness));
                }
            }
        }

        // Ambient Occlusion Texture
        if (material->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION) > 0)
        {
            aiString path;
            if (material->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &path) ==
                AI_SUCCESS)
            {
                vector<Texture> ambientOcclusionMap =
                    LoadTexture(path.C_Str(), textureName, material,
                                aiTextureType_AMBIENT_OCCLUSION);
                for (auto& ambientOcclusion : ambientOcclusionMap){
                    localMesh.textures.push_back(std::move(ambientOcclusion));
                }
            }
        }

        // Unknown: that does not match any of the definitions above
        if (material->GetTextureCount(aiTextureType_UNKNOWN) > 0)
        {
            aiString path;
            if (material->GetTexture(aiTextureType_UNKNOWN, 0, &path) == AI_SUCCESS)
            {
                vector<Texture> unknownMap = LoadTexture(
                    path.C_Str(), textureName, material, aiTextureType_UNKNOWN);
                for (auto& unknown : unknownMap){
                    localMesh.textures.push_back(std::move(unknown));
                }
            }
        }
    }
    
    return localMesh;
}

vector<Texture> AssetService::LoadTexture(const string &path,
                                          const string &name, aiMaterial *mat,
                                          aiTextureType type)
{
    vector<Texture> textures;
    for (uint32_t i = 0; i < mat->GetTextureCount(type); ++i)
    {
        // Check if texture was loaded before and if so, continue to next
        // iteration: skip loading a new texture
        bool skip = false;
        if (texturesLoaded_.find(name) != texturesLoaded_.end())
        {
            //textures.push_back(texturesLoaded_[name]);
            skip = true;
        }

        // If texture hasn't been loaded already, load it
        if (!skip)
        {
            Texture texture(path, Texture::InterpolationMode::kLinear);
            textures.push_back(std::move(texture));
            // Store it as texture loaded for entire model, to ensure not to
            // load duplicate textures
            texturesLoaded_[name] = std::move(texture);
        }
    }

    return textures;
}

void AssetService::OnInit()
{
    LoadMesh("resources/models/cube.obj", "cube", "");
    LoadMesh("resources/models/plane.obj", "plane", "");
    LoadMesh("resources/models/stanford_bunny.obj", "bunny", "");
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