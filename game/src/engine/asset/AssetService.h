#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "engine/core/gfx/Texture.h"
#include "engine/gui/OnGuiEvent.h"
#include "engine/input/InputService.h"
#include "engine/render/Mesh.h"
#include "engine/service/Service.h"

struct aiScene;
struct aiMesh;
struct aiNode;
class InputService;

class AssetService final : public Service, public IEventSubscriber<OnGuiEvent>
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

    // From IEventSubscriber<OnGuiEvent>
    void OnGui() override;

  private:
    jss::object_ptr<InputService> input_service_;

    std::unordered_map<std::string, std::unique_ptr<Texture>> textures_;
    std::unordered_map<std::string, std::unique_ptr<Mesh>> meshes_;
    bool show_menu_;

    void ProcessMesh(aiMesh *mesh, const std::string &name);
    void LoadAssetFile(const std::string &path);
    void DrawDebugMeshGui();
    void DrawDebugTextureGui();
};