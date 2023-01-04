#pragma once

#include <memory>
#include <vector>

#include "core/gfx/ShaderProgram.h"

class ShaderManager
{
  public:
    enum class ShaderType : uint8_t
    {
        // No texture, no lighting
        kBasic = 0,

        // Textured, no lighting
        kTextured = 1,

        // Material, textured, lighting
        kPhong = 2,

        // Earth has some specular maps, normal maps, other fun stuff
        kEarth = 3,

        // Should always be last
        kCount
    };

    struct ShaderData
    {
        ShaderType type;
        std::unique_ptr<ShaderProgram> shader;
    };

    static void Init();
    static void Destroy();
    static ShaderManager& Instance();

    // No copying allowed
    ShaderManager(const ShaderManager&) = delete;
    ShaderManager operator=(const ShaderManager&) = delete;

    ShaderProgram& GetProgram(ShaderType type);
    std::vector<std::unique_ptr<ShaderData>>& GetPrograms();

  protected:
    ShaderManager();

  private:
    static ShaderManager* instance_;
    std::vector<std::unique_ptr<ShaderData>> programs_;

    void CreateShaders();

    void AddProgram(ShaderType type, std::unique_ptr<ShaderProgram> program);
};