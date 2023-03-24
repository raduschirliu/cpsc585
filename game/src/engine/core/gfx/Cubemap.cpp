#include "engine/core/gfx/Cubemap.h"

#include <stb/stb_image.h>

#include "engine/core/debug/Assert.h"
#include "engine/core/debug/Log.h"

static constexpr uint32_t kGlFirstTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
static constexpr uint32_t kGlLastTarget = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;

Cubemap::Cubemap() : handle_()
{
}

void Cubemap::LoadTexture(uint32_t target, const std::string& path)
{
    ASSERT_MSG(target >= kGlFirstTarget && target <= kGlLastTarget,
               "Invalid cubemap texture target");

    int num_components;
    stbi_set_flip_vertically_on_load(false);

    int width, height;
    unsigned char* data =
        stbi_load(path.c_str(), &width, &height, &num_components, 0);

    ASSERT_MSG(data, "Texture must be loaded");
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
            ASSERT_ALWAYS("Invalid Texture Format");
            break;
    };

    // Loads texture data into bound texture
    glTexImage2D(target, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE,
                 data);
    stbi_image_free(data);
}

void Cubemap::Finalize()
{
    Bind();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Cubemap::Bind(uint32_t slot) const
{
    ASSERT_MSG(GL_TEXTURE0 + slot <= GL_TEXTURE31,
               "Exceeded max bound texture amount");

    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, handle_);
}
