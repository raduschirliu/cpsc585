#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "engine/core/gfx/Texture.h"
#include "engine/render/Mesh.h"
#include "engine/service/Service.h"

struct aiScene;
struct aiMesh;
struct aiNode;

class AssetService final : public Service
{
  public:
    void LoadMesh(const std::string &path, const std::string &name);
    const Mesh &GetMesh(const std::string &name);

    void LoadTexture(const std::string &path, const std::string &name);
    const Texture &GetTexture(const std::string &name);

    // From Service
    void OnInit() override;
    void OnStart(ServiceProvider &service_provider) override;
    void OnUpdate() override;
    void OnCleanup() override;
    std::string_view GetName() const override;

  private:
    std::unordered_map<std::string, std::unique_ptr<Texture>> textures_;
    std::unordered_map<std::string, Mesh> meshes_;

    void ProcessNode(const std::string &path, const std::string &name,
                     aiNode *node, const aiScene *scene);
    Mesh ProcessMesh(aiNode *node, aiMesh *mesh, const aiScene *scene);
};