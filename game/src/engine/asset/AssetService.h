#pragma once

#include <assimp/postprocess.h>  // Post processing flags
#include <assimp/scene.h>        // Output data structure

#include <assimp/Importer.hpp>  // C++ importer interface
#include <string>
#include <unordered_map>
#include <vector>

#include "engine/core/gfx/Texture.h"
#include "engine/render/Mesh.h"
#include "engine/service/Service.h"

class AssetService final : public Service
{
  public:
    void LoadMesh(const std::string &path, const std::string &name,
                  const std::string &textureName);
    const Mesh &GetMesh(const std::string &name);

    // From Service
    void OnInit() override;
    void OnStart(ServiceProvider &service_provider) override;
    void OnUpdate() override;
    void OnCleanup() override;
    std::string_view GetName() const override;

  private:
    std::unordered_map<std::string, Texture> texturesLoaded_;
    std::unordered_map<std::string, Mesh> meshes_;

    void ProcessNode(const std::string &path, const std::string &name,
                     const std::string &textureName, aiNode *node,
                     const aiScene *scene);
    Mesh ProcessMesh(aiNode *node, const std::string &textureName, aiMesh *mesh,
                     const aiScene *scene);
    std::vector<Texture> LoadTexture(const std::string &path,
                                     const std::string &name, aiMaterial *mat,
                                     aiTextureType type);
};
