#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <string>

#include "GLHandles.h"

class Texture
{
  public:
    enum class Slot : GLenum
    {
        kTexture0 = GL_TEXTURE0,
        kTexture1 = GL_TEXTURE1,
        kTexture2 = GL_TEXTURE2,
        kTexture3 = GL_TEXTURE3
    };

    enum class InterpolationMode : GLint
    {
        kLinear = GL_LINEAR,
    };

    static void Unbind();

    Texture(std::string path, InterpolationMode interpolation_mode);

    // Although uint (i.e. uvec2) might make more sense here, went with int
    // (i.e. ivec2) under the assumption that most students will want to work
    // with ints, not uints, in main.cpp
    glm::ivec2 GetDimensions() const;
    void Bind(Slot slot = Slot::kTexture0) const;

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
