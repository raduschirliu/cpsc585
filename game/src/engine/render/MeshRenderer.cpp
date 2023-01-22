#include "engine/render/MeshRenderer.h"

#include <assimp/postprocess.h>  // Post processing flags
#include <assimp/scene.h>        // Output data structure

#include <assimp/Importer.hpp>  // C++ importer interface
#include <glm/gtc/matrix_transform.hpp>

#include "engine/core/debug/Log.h"
#include "engine/render/RenderService.h"
#include "engine/scene/Entity.h"

using glm::mat4;

const Mesh& MeshRenderer::GetMesh() const
{
    return mesh_;
}

const mat4& MeshRenderer::GetModelMatrix() const
{
    return transform_->GetModelMatrix();
}

void MeshRenderer::OnInit(const ServiceProvider& service_provider)
{
    // TEMP: Load mesh using assimp
    Assimp::Importer importer;
    const aiScene* cube_scene = importer.ReadFile(
        "resources/models/cube.obj",
        aiProcess_CalcTangentSpace | aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

    ASSERT_MSG(cube_scene, "Import must be succesful");
    ASSERT_MSG(cube_scene->HasMeshes(), "Must have mesh");

    for (uint32_t i = 0; i < cube_scene->mNumMeshes; i++)
    {
        aiMesh* mesh = cube_scene->mMeshes[i];

        for (uint32_t j = 0; j < mesh->mNumVertices; j++)
        {
            aiVector3D& vertex = mesh->mVertices[j];
            mesh_.vertices.push_back(
                Vertex(glm::vec3(vertex.x, vertex.y, vertex.z)));
        }

        for (uint32_t j = 0; j < mesh->mNumFaces; j++)
        {
            aiFace& face = mesh->mFaces[j];
            mesh_.indices.insert(mesh_.indices.end(), &face.mIndices[0],
                                 &face.mIndices[face.mNumIndices]);
        }
    }

    Log::info("Loaded mesh with {} indices and {} faces", mesh_.indices.size(),
              mesh_.vertices.size());

    // Get dependencies
    render_service_ = &service_provider.GetService<RenderService>();
    render_service_->RegisterRenderable(*this);

    transform_ = &GetEntity().GetComponent<Transform>();
}

std::string_view MeshRenderer::GetName() const
{
    return "MeshRenderer";
}
