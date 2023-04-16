#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <string>

#include "GLHandles.h"

class Texture
{
  public:
    enum class InterpolationMode : GLint
    {
        kLinear = GL_LINEAR,
    };

    static void Unbind();

    Texture();

    void LoadFromFile(std::string path, bool srgb = true,
                      bool flip_vertically = false);
    void UpdateParams();

    glm::uvec2 GetDimensions() const;
    void Bind(uint32_t slot = 0) const;
    const TextureHandle& GetHandle() const;
    void* GetGuiHandle() const;

  private:
    TextureHandle handle_;
    std::string path_;
    InterpolationMode interpolation_;
    glm::uvec2 size_;

    GLint GetInterpolationMode() const;
};
