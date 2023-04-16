#include "Texture.h"

#include <stb/stb_image.h>

#include "engine/core/debug/Assert.h"

using glm::uvec2;

Texture::Texture()
    : handle_(),
      path_(""),
      interpolation_(InterpolationMode::kLinear),
      size_(0, 0)
{
}

void Texture::LoadFromFile(std::string path, bool srgb, bool flip_vertically)
{
    path_ = path;

    if (flip_vertically)
    {
        stbi_set_flip_vertically_on_load(true);
    }

    int num_components;
    int width, height;
    unsigned char* data =
        stbi_load(path.c_str(), &width, &height, &num_components, 0);

    ASSERT_MSG(data, "Texture must be loaded");
    size_ = uvec2(width, height);

    // Set alignment to be 1
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    Bind();

    // Set number of components by format of the texture
    GLuint data_format = GL_RGB;
    GLuint internal_format = GL_SRGB;

    switch (num_components)
    {
        case 4:
            data_format = GL_RGBA;
            internal_format = srgb ? GL_SRGB_ALPHA : GL_RGBA;
            break;

        case 3:
            data_format = GL_RGB;
            internal_format = srgb ? GL_SRGB : GL_RGB;
            break;

        case 2:
            data_format = GL_RG;
            internal_format = GL_RG;
            break;

        case 1:
            data_format = GL_RED;
            internal_format = GL_RG;
            break;

        default:
            ASSERT_ALWAYS("Invalid Texture Format");
            break;
    };
    // Loads texture data into bound texture
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0,
                 data_format, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    UpdateParams();

    // Clean up
    Unbind();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);  // Return to default alignment
}

void Texture::UpdateParams()
{
    Bind();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GetInterpolationMode());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                    GetInterpolationMode());
}

uvec2 Texture::GetDimensions() const
{
    return size_;
}

void Texture::Bind(uint32_t slot) const
{
    ASSERT_MSG(GL_TEXTURE0 + slot <= GL_TEXTURE31,
               "Exceeded max bound texture amount");

    glActiveTexture(GL_TEXTURE0 + slot);
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

void* Texture::GetGuiHandle() const
{
    return handle_.ValueRaw();
}