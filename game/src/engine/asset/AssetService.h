#pragma once

#include <unordered_map>
#include <vector>
#include <string>

#include "engine/core/gfx/Texture.h"
#include "engine/render/Mesh.h"
#include "engine/service/Service.h"

#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h>       // Output data structure

#include <assimp/Importer.hpp>  // C++ importer interface

class AssetService final : public Service
{
  public:
    void LoadModel(const std::string &path, const std::string &name);
    const Mesh& GetMesh(const std::string& name);

    // From Service
    void OnInit() override;
    void OnStart(ServiceProvider &service_provider) override;
    void OnUpdate() override;
    void OnCleanup() override;
    std::string_view GetName() const override;

  private:
    std::vector<Texture> texturesLoaded_;
    std::unordered_map<std::string, Mesh> meshes_;

    void ProcessNode(const std::string &path, const std::string &name,
                     aiNode *node, const aiScene *scene);
    Mesh ProcessMesh(aiNode *node, aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> LoadTexture(const std::string &path, aiMaterial *mat,
                                     aiTextureType type);
    uint32_t TextureFromFile(const std::string &path);
};