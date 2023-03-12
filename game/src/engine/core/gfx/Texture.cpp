#include "Texture.h"

#include <stb/stb_image.h>

#include <iostream>

#include "engine/core/debug/Assert.h"

Texture::Texture(std::string path, InterpolationMode interpolation_mode)
    : handle_(),
      path_(path),
      interpolation_(interpolation_mode)
{
    int num_components;
    stbi_set_flip_vertically_on_load(false);
    unsigned char* data =
        stbi_load(path.c_str(), &width_, &height_, &num_components, 0);

    ASSERT_MSG(data, "Texture must be loaded");

    // Set alignment to be 1
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    Bind();

    // Set number of components by format of the texture
    GLuint format = GL_RGB;
    switch (num_components)
    {
        case 4:
            format = GL_RGBA;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 2:
            format = GL_RG;
            break;
        case 1:
            format = GL_RED;
            break;
        default:
            std::cout << "Invalid Texture Format" << std::endl;
            break;
    };
    // Loads texture data into bound texture
    glTexImage2D(GL_TEXTURE_2D, 0, format, width_, height_, 0, format,
                 GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GetInterpolationMode());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                    GetInterpolationMode());

    // Clean up
    Unbind();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);  // Return to default alignment
}

glm::ivec2 Texture::GetDimensions() const
{
    return glm::uvec2(width_, height_);
}

void Texture::Bind(Texture::Slot slot) const
{
    glActiveTexture(static_cast<GLenum>(slot));
    glBindTexture(GL_TEXTURE_2D, handle_);
}

void Texture::Unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

GLint Texture::GetInterpolationMode() const
{
    return static_cast<GLint>(interpolation_);
}

const TextureHandle& Texture::GetHandle() const
{
    return handle_;
}