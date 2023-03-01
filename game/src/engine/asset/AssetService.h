#pragma once

#include <assimp/postprocess.h>  // Post processing flags
#include <assimp/scene.h>        // Output data structure

#include <assimp/Importer.hpp>  // C++ importer interface
#include <string>
#include <unordered_map>
#include <vector>

#include "engine/core/gfx/Texture.h"
#include "engine/render/Material.h"
#include "engine/render/Mesh.h"
#include "engine/service/Service.h"

class AssetService final : public Service
{
  public:
    void LoadMesh(const std::string &path, const std::string &name);
    const Mesh &GetMesh(const std::string &name);
    void LoadTexture(const std::string &path, aiMesh *mesh,
                     const std::string &name);
    const Texture &GetTexture(const std::string &name);
    void LoadMaterial(const std::string &path, const aiScene *scene, aiMesh *mesh,
                      const std::string &name);

    // From Service
    void OnInit() override;
    void OnStart(ServiceProvider &service_provider) override;
    void OnUpdate() override;
    void OnCleanup() override;
    std::string_view GetName() const override;

  private:
    std::unordered_map<std::string, Texture> texturesLoaded_;
    std::unordered_map<std::string, Mesh> meshes_;
    std::unordered_map<std::string, MaterialProperties> materials_;
    float shininess_;

    void ProcessNode(const std::string &path, const std::string &name,
                     aiNode *node, const aiScene *scene);
    Mesh ProcessMesh(aiNode *node, aiMesh *mesh, const aiScene *scene);
    void ProcessTexture(const std::string &path, const std::string &name,
                        aiMaterial *mat, aiTextureType type);
    void ProcessMaterial(aiColor3D color, const std::string &name,
                         aiMaterial *mat);
};