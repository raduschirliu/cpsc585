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

    Texture(std::string path,
            InterpolationMode interpolation_mode = InterpolationMode::kLinear,
            bool flip_vertically = false);

    glm::uvec2 GetDimensions() const;
    void Bind(uint32_t slot = 0) const;
    const TextureHandle& GetHandle() const;
    void* GetGuiHandle() const;

  private:
    TextureHandle handle_;
    std::string path_;
    InterpolationMode interpolation_;

    // Although uint might make more sense here, went with int under the
    // assumption that most students will want to work with ints, not uints, in
    // main.cpp
    int width_;
    int height_;

    GLint GetInterpolationMode() const;
};
