#include "engine/asset/AssetService.h"

#include "engine/core/debug/Log.h"
#include "engine/scene/Entity.h"

using namespace std;

void AssetService::LoadModel(const string &path, const string &name)
{
    // Model importing
    Assimp::Importer importer;

    // Process the desired calculation for the model
    unsigned int flag;
    flag = aiProcess_Triangulate |  // Transform all the model's primitive
                                    // shapes to triangles
           aiProcess_JoinIdenticalVertices |  // Identifies and joins identical
                                              // vertex data sets within all
                                              // imported meshes
           aiProcess_CalcTangentSpace |       // Calculate the tangents and
                                         // bitangents for the imported meshes
           aiProcess_FlipWindingOrder;  // Adjust the output face winding order
                                        // to be clockwise

    const aiScene *scene = importer.ReadFile(path, flag);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->mRootNode)
    {
        ASSERT_MSG(false, importer.GetErrorString());
    }

    ProcessNode(path, name, scene->mRootNode, scene);
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
        aiFace face = mesh->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; ++j)
        {
            localMesh.indices.emplace_back(face.mIndices[j]);
        }
    }

    // Texture (Material) information
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

    // // Diffuse Texture: combined with the result of the diffuse lighting
    // equation if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0){
    //     aiString path;
    //     if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) ==
    //     AI_SUCCESS){
    //         vector<Texture> diffuseMap = LoadTexture(path, material,
    //         aiTextureType_DIFFUSE); textures.insert(textures.end(),
    //         diffuseMap.begin(), diffuseMap.end());
    //     }
    // }

    // // Specular Texture: combined with the result of the specular lighting
    // equation if (material->GetTextureCount(aiTextureType_SPECULAR) > 0){
    //     aiString path;
    //     if (material->GetTexture(aiTextureType_SPECULAR, 0, &path) ==
    //     AI_SUCCESS){
    //         vector<Texture> specularMap = LoadTexture(path, material,
    //         aiTextureType_SPECULAR); textures.insert(textures.end(),
    //         specularMap.begin(), specularMap.end());
    //     }
    // }

    // // Ambient Texture: combined with the result of the ambient lighting
    // equation if (material->GetTextureCount(aiTextureType_AMBIENT) > 0){
    //     aiString path;
    //     if (material->GetTexture(aiTextureType_AMBIENT, 0, &path) ==
    //     AI_SUCCESS){
    //         vector<Texture> ambientMap = LoadTexture(path, material,
    //         aiTextureType_AMBIENT); textures.insert(textures.end(),
    //         ambientMap.begin(), ambientMap.end());
    //     }
    // }

    // // Emissive Texture: added to the result of the lighting calculation (X
    // influenced by incoming light) if
    // (material->GetTextureCount(aiTextureType_EMISSIVE) > 0){
    //     aiString path;
    //     if (material->GetTexture(aiTextureType_EMISSIVE, 0, &path) ==
    //     AI_SUCCESS){
    //         vector<Texture> emissiveMap = LoadTexture(path, material,
    //         aiTextureType_EMISSIVE); textures.insert(textures.end(),
    //         emissiveMap.begin(), emissiveMap.end());
    //     }
    // }

    // // Height Texture: higher grey-scale values stand for higher elevations
    // from the base height if (material->GetTextureCount(aiTextureType_HEIGHT)
    // > 0){
    //     aiString path;
    //     if (material->GetTexture(aiTextureType_HEIGHT, 0, &path) ==
    //     AI_SUCCESS){
    //         vector<Texture> heightMap = LoadTexture(path, material,
    //         aiTextureType_HEIGHT); textures.insert(textures.end(),
    //         heightMap.begin(), heightMap.end());
    //     }
    // }

    // // Normal Texture: a (tangent space) normal-map
    // if (material->GetTextureCount(aiTextureType_NORMALS) > 0){
    //     aiString path;
    //     if (material->GetTexture(aiTextureType_NORMALS, 0, &path) ==
    //     AI_SUCCESS){
    //         vector<Texture> normalsMap = LoadTexture(path, material,
    //         aiTextureType_NORMALS); textures.insert(textures.end(),
    //         normalsMap.begin(), normalsMap.end());
    //     }
    // }

    // // Shininess Texture: defines the glossiness of the material
    // if (material->GetTextureCount(aiTextureType_SHININESS) > 0){
    //     aiString path;
    //     if (material->GetTexture(aiTextureType_SHININESS, 0, &path) ==
    //     AI_SUCCESS){
    //         vector<Texture> shininessMap = LoadTexture(path, material,
    //         aiTextureType_SHININESS); textures.insert(textures.end(),
    //         shininessMap.begin(), shininessMap.end());
    //     }
    // }

    // // Opacity Texture (Transparent): defines per-pixel opacity (white ==
    // opaque and black == transparent) if
    // (material->GetTextureCount(aiTextureType_OPACITY) > 0){
    //     aiString path;
    //     if (material->GetTexture(aiTextureType_OPACITY, 0, &path) ==
    //     AI_SUCCESS){
    //         vector<Texture> opacityMap = LoadTexture(path, material,
    //         aiTextureType_OPACITY); textures.insert(textures.end(),
    //         opacityMap.begin(), opacityMap.end());
    //     }
    // }

    // // Displacement Texture: higher color values stand for higher vertex
    // displacement if (material->GetTextureCount(aiTextureType_DISPLACEMENT) >
    // 0){
    //     aiString path;
    //     if (material->GetTexture(aiTextureType_DISPLACEMENT, 0, &path) ==
    //     AI_SUCCESS){
    //         vector<Texture> displacementMap = LoadTexture(path, material,
    //         aiTextureType_DISPLACEMENT); textures.insert(textures.end(),
    //         displacementMap.begin(), displacementMap.end());
    //     }
    // }

    // // LightMap Texture: cover both lightmaps and dedicated ambient occlusion
    // maps
    // //                   contains a scaling value for the final color value
    // of a pixel if (material->GetTextureCount(aiTextureType_LIGHTMAP) > 0){
    //     aiString path;
    //     if (material->GetTexture(aiTextureType_LIGHTMAP, 0, &path) ==
    //     AI_SUCCESS){
    //         vector<Texture> lightMap = LoadTexture(path, material,
    //         aiTextureType_LIGHTMAP); textures.insert(textures.end(),
    //         lightMap.begin(), lightMap.end());
    //     }
    // }

    // // BaseColor Texture
    // if (material->GetTextureCount(aiTextureType_BASE_COLOR) > 0){
    //     aiString path;
    //     if (material->GetTexture(aiTextureType_BASE_COLOR, 0, &path) ==
    //     AI_SUCCESS){
    //         vector<Texture> baseColorMap = LoadTexture(path, material,
    //         aiTextureType_BASE_COLOR); textures.insert(textures.end(),
    //         baseColorMap.begin(), baseColorMap.end());
    //     }
    // }

    // // Emissive Color Texture
    // if (material->GetTextureCount(aiTextureType_EMISSION_COLOR) > 0){
    //     aiString path;
    //     if (material->GetTexture(aiTextureType_EMISSION_COLOR, 0, &path) ==
    //     AI_SUCCESS){
    //         vector<Texture> emmissionColorMap = LoadTexture(path, material,
    //         aiTextureType_EMISSION_COLOR); textures.insert(textures.end(),
    //         emmissionColorMap.begin(), emmissionColorMap.end());
    //     }
    // }

    // // Metalness Texture
    // if (material->GetTextureCount(aiTextureType_METALNESS) > 0){
    //     aiString path;
    //     if (material->GetTexture(aiTextureType_METALNESS, 0, &path) ==
    //     AI_SUCCESS){
    //         vector<Texture> metalnessMap = LoadTexture(path, material,
    //         aiTextureType_METALNESS); textures.insert(textures.end(),
    //         metalnessMap.begin(), metalnessMap.end());
    //     }
    // }

    // // Roughness Texture
    // if (material->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS) > 0){
    //     aiString path;
    //     if (material->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &path)
    //     == AI_SUCCESS){
    //         vector<Texture> diffuseRoughnessMap = LoadTexture(path, material,
    //         aiTextureType_DIFFUSE_ROUGHNESS); textures.insert(textures.end(),
    //         diffuseRoughnessMap.begin(), diffuseRoughnessMap.end());
    //     }
    // }

    // // Ambient Occlusion Texture
    // if (material->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION) > 0){
    //     aiString path;
    //     if (material->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &path)
    //     == AI_SUCCESS){
    //         vector<Texture> ambientOcclusionMap = LoadTexture(path, material,
    //         aiTextureType_AMBIENT_OCCLUSION); textures.insert(textures.end(),
    //         ambientOcclusionMap.begin(), ambientOcclusionMap.end());
    //     }
    // }

    // // Unknown: that does not match any of the definitions above
    // if (material->GetTextureCount(aiTextureType_UNKNOWN) > 0){
    //     aiString path;
    //     if (material->GetTexture(aiTextureType_UNKNOWN, 0, &path) ==
    //     AI_SUCCESS){
    //         vector<Texture> unknownMap = LoadTexture(path, material,
    //         aiTextureType_UNKNOWN); textures.insert(textures.end(),
    //         unknownMap.begin(), unknownMap.end());
    //     }
    // }

    return localMesh;
}

// vector<Texture> AssetService::LoadTexture(const string &path, aiMaterial
// *mat, aiTextureType type)
// {
//     vector<Texture> textures;
//     for (uint32_t i = 0; i < mat->GetTextureCount(type); ++i){
//         // Check if texture was loaded before and if so, continue to next
//         iteration: skip loading a new texture bool skip = false; for
//         (uint32_t j = 0; j < texturesLoaded.size(); ++j){
//             if (strcmp(texturesLoaded[j].path.data(), path.C_Str()) == 0){
//                 textures.push_back(texturesLoaded[j]);
//                 // A texture with the same filepath has already been loaded,
//                 continue to next one. (optimization) skip = true; break;
//             }
//         }

//         // If texture hasn't been loaded already, load it
//         if (!skip){
//             Texture texture;
//             texture.id = TextureFromFile(path.C_Str());
//             texture.path = path.C_Str();
//             textures.push_back(texture);
//             // Store it as texture loaded for entire model, to ensure not to
//             load duplicate textures texturesLoaded.push_back(texture);
//         }
//     }

//     return textures;
// }

// uint32_t AssetService::TextureFromFile(const string &path)
// {
//     uint32_t textureID;
//     glGenTextures(1, &textureID);

//     int width, height, nrComponents;
//     unsigned char *data = stbi_load(path.c_str(), &width, &height,
//     &nrComponents, 0); if (data){
//         GLenum format;
//         if (nrComponents == 1) format = GL_RED;
//         else if (nrComponents == 3) format = GL_RGB;
//         else if (nrComponents == 4) format = GL_RGBA;

//         glBindTexture(GL_TEXTURE_2D, textureID);
//         glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
//         GL_UNSIGNED_BYTE, data); glGenerateMipmap(GL_TEXTURE_2D);

//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
//         GL_LINEAR_MIPMAP_LINEAR); glTexParameteri(GL_TEXTURE_2D,
//         GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//         stbi_image_free(data);
//     } else {
//         std::cout << "Texture failed to load at path: " << path << std::endl;
//         stbi_image_free(data);
//     }

//     return textureID;
// }

void AssetService::OnInit()
{
    // A mash must be generated and stored in meshes_ after the model is loaded
    // successfully
    LoadModel("resources/models/cube.obj", "cube");
    LoadModel("resources/models/plane.obj", "plane");
    LoadModel("resources/models/stanford_bunny.obj", "bunny");
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