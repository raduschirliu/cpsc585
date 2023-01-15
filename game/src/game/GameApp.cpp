#include "game/GameApp.h"

#include <yaml-cpp/yaml.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>

#include "engine/core/debug/Assert.h"
#include "engine/core/debug/Log.h"

using glm::ivec2;
using std::string;

using namespace std;

GameApp::GameApp()
{
}

void LoadModel::LoadModel(const string &path, const Ref<Scene>& pScene)
{
    // Model importing
    Assimp::Importer importer;

    // Process the desired calculation for the model
    unsigned int flag;
    flag = aiProcess_Triangulate |          // Transform all the model's primitive shapes to triangles
        aiProcess_JoinIdenticalVertices |   // Identifies and joins identical vertex data sets within all imported meshes
        aiProcess_CalcTangentSpace |        // Calculate the tangents and bitangents for the imported meshes
        aiProcess_GenNormals |              // Create normal vectors for each vertex if the model doesn't contain normal vectors
        aiProcess_MakeLeftHanded |          // Convert all the imported data to a left-handed coordinate space
        aiProcess_FlipWindingOrder;         // Adjust the output face winding order to be clockwise

    const aiScene* scene = importer.ReadFile(path, flag);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
        QCAT_ASSERT(false, importer.GetErrorString());
    }

    ProcessNode(path, scene->mRootNode, scene, pScene);    
}

/*
 * .  : Directly access the members of the class
 * -> : Approach the member through a pointer
 */
void LoadModel::ProcessNode(const string &path, aiNode *node, const aiScene *scene, const Ref<Scene>& pScene)
{
    string nodeName = node->mName.C_Str();
    Entity entity = pScene->CreateEntity(nodeName);
    glm::mat4 transform = Utils::ConvertToGlm(node->mTransformation);   // A transform value relative to its parent node
    entity.GetComponent<Transform>().SetTransform(transform);
    
    if (node->mNumMeshes != 0){
        entity.AddComponent<Mesh>();
        entity.AddComponent<Material>();
    } // Else, that node has only a location value without mesh and material component
    
    // The first mesh is for the node entity to have, and from the second mesh, create a new entity and assign it as a child node
    // -> To treat each mesh with the name "Name of Parent Node + Index" since it may not have a name
    for (uint32_t i = 0; i < node->mNumMeshes; ++i){
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        if (i < 1){
            entity.GetComponent<Mesh>().vertexArray = ProcessMesh(node, mesh, scene);
            entity.GetComponent<Material>().material = ProcessMaterial(mesh, scene);
        } else{
            Entity sub = pScene->CreateEntity(mesh->mName.data);
            sub.SetParent(&entity);
            sub.AddComponent<Mesh>().vertexArray = ProcessMesh(node, mesh, scene);
            sub.AddComponent<Material>().material = ProcessMaterial(mesh, scene);
        }
    }

    // Do the same for each of its children
    for (uint32_t i = 0; i < node->mNumChildren; ++i){
        ProcessNode(path, node->mChildren[i], scene, pScene);
    }
}

/*
 * push_back()      : No internal constructor (Create a temporary object from the outside)
 * emplace_back()   : Create its own objects internally using the constructor
 */
Mesh LoadModel::ProcessMesh(aiNode *node, aiMesh *mesh, const aiScene *scene)
{
    vector<Vertex> vertices;
    vector<uint32_t> indices;
    
    // Vertex information
    for (uint32_t i = 0; i < mesh->mNumVertices; ++i){
        Vertex vertex;
        glm::vec3 vector;

        // Position
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;

        // Normal
        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.normal = vector;

        // TextureCoord: Maximum 8 texture
        if (mesh->mTextureCoords[0]){
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texcoords = vec;
            
            // Tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.tangent = vector;

            // BiTangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.bitangent = vector;
        } else vertex.texcoords = glm::vec2(0.0f, 0.0f);

        vertices.emplace_back(vertex);
    }

    // Index information: May vary depending on the value of the Winding flag
    for (uint32_t i = 0; i < mesh->mNumFaces; ++i){
        aiFace face = mesh->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; ++j){
            indices.emplace_back(face.mIndices[j]);
        }
    }

    return ;
}

Material LoadModel::ProcessMaterial(aiMesh *mesh, const aiScene *scene)
{
    string fullpath;
    Material material;
    aiMaterial* aimaterial = scene->mMaterials[mesh->mMaterialIndex];

    // Diffuse Texture: combined with the result of the diffuse lighting equation
    if (aimaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0){
        aiString path;
        if (aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS){
            fullpath = path.data;
        }
    }

    // Specular Texture: combined with the result of the specular lighting equation
    if (aimaterial->GetTextureCount(aiTextureType_SPECULAR) > 0){
        aiString path;
        if (aimaterial->GetTexture(aiTextureType_SPECULAR, 0, &path) == AI_SUCCESS){
            fullpath = path.data;
        }
    }

    // Ambient Texture: combined with the result of the ambient lighting equation
    if (aimaterial->GetTextureCount(aiTextureType_AMBIENT) > 0){
        aiString path;
        if (aimaterial->GetTexture(aiTextureType_AMBIENT, 0, &path) == AI_SUCCESS){
            fullpath = path.data;
        }
    }

    // Emissive Texture: added to the result of the lighting calculation (X influenced by incoming light)
    if (aimaterial->GetTextureCount(aiTextureType_EMISSIVE) > 0){
        aiString path;
        if (aimaterial->GetTexture(aiTextureType_EMISSIVE, 0, &path) == AI_SUCCESS){
            fullpath = path.data;
        }
    }

    // Height Texture: higher grey-scale values stand for higher elevations from the base height
    if (aimaterial->GetTextureCount(aiTextureType_HEIGHT) > 0){
        aiString path;
        if (aimaterial->GetTexture(aiTextureType_HEIGHT, 0, &path) == AI_SUCCESS){
            fullpath = path.data;
        }
    }

    // Normal Texture: a (tangent space) normal-map
    if (aimaterial->GetTextureCount(aiTextureType_NORMALS) > 0){
        aiString path;
        if (aimaterial->GetTexture(aiTextureType_NORMALS, 0, &path) == AI_SUCCESS){
            fullpath = path.data;
        }
    }

    // Shininess Texture: defines the glossiness of the material
    if (aimaterial->GetTextureCount(aiTextureType_SHININESS) > 0){
        aiString path;
        if (aimaterial->GetTexture(aiTextureType_SHININESS, 0, &path) == AI_SUCCESS){
            fullpath = path.data;
        }
    }

    // Opacity Texture (Transparent): defines per-pixel opacity (white == opaque and black == transparent)
    if (aimaterial->GetTextureCount(aiTextureType_OPACITY) > 0){
        aiString path;
        if (aimaterial->GetTexture(aiTextureType_OPACITY, 0, &path) == AI_SUCCESS){
            fullpath = path.data;
        }
    }

    // Displacement Texture: higher color values stand for higher vertex displacement
    if (aimaterial->GetTextureCount(aiTextureType_DISPLACEMENT) > 0){
        aiString path;
        if (aimaterial->GetTexture(aiTextureType_DISPLACEMENT, 0, &path) == AI_SUCCESS){
            fullpath = path.data;
        }
    }

    // LightMap Texture: cover both lightmaps and dedicated ambient occlusion maps
    //                   contains a scaling value for the final color value of a pixel
    if (aimaterial->GetTextureCount(aiTextureType_LIGHTMAP) > 0){
        aiString path;
        if (aimaterial->GetTexture(aiTextureType_LIGHTMAP, 0, &path) == AI_SUCCESS){
            fullpath = path.data;
        }
    }

    // BaseColor Texture
    if (aimaterial->GetTextureCount(aiTextureType_BASE_COLOR) > 0){
        aiString path;
        if (aimaterial->GetTexture(aiTextureType_BASE_COLOR, 0, &path) == AI_SUCCESS){
            fullpath = path.data;
        }
    }

    // Emissive Color Texture
    if (aimaterial->GetTextureCount(aiTextureType_EMISSION_COLOR) > 0){
        aiString path;
        if (aimaterial->GetTexture(aiTextureType_EMISSION_COLOR, 0, &path) == AI_SUCCESS){
            fullpath = path.data;
        }
    }

    // Metalness Texture
    if (aimaterial->GetTextureCount(aiTextureType_METALNESS) > 0){
        aiString path;
        if (aimaterial->GetTexture(aiTextureType_METALNESS, 0, &path) == AI_SUCCESS){
            fullpath = path.data;
        }
    }

    // Roughness Texture
    if (aimaterial->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS) > 0){
        aiString path;
        if (aimaterial->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &path) == AI_SUCCESS){
            fullpath = path.data;
        }
    }

    // Ambient Occlusion Texture
    if (aimaterial->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION) > 0){
        aiString path;
        if (aimaterial->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &path) == AI_SUCCESS){
            fullpath = path.data;
        }
    }
    
    // Unknown: that does not match any of the definitions above
    if (aimaterial->GetTextureCount(aiTextureType_UNKNOWN) > 0){
        aiString path;
        if (aimaterial->GetTexture(aiTextureType_UNKNOWN, 0, &path) == AI_SUCCESS){
            fullpath = path.data;
        }
    }

    return ;
}

void GameApp::Init()
{
    GetWindow().SetSize(ivec2(1280, 720));

    // Model importing test
    Assimp::Importer importer;
    const aiScene* cube_scene =
        importer.ReadFile("resources/models/cube.obj", 0);
    ASSERT_MSG(cube_scene, "Import must be succesful");

    // Yaml parsing test
    YAML::Node root = YAML::LoadFile("resources/scenes/test.yaml");
    Log::debug("someRootNode.someChildNode = {}",
               root["someRootNode"]["someChildNode"].as<string>());
}