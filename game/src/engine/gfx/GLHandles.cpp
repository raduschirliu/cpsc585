#include "engine/gfx/GLHandles.h"

#include <algorithm>  // For std::swap

ShaderHandle::ShaderHandle(GLenum type) : shaderID(glCreateShader(type))
{
}

ShaderHandle::ShaderHandle(ShaderHandle&& other) noexcept
    : shaderID(std::move(other.shaderID))
{
    other.shaderID = 0;
}

ShaderHandle& ShaderHandle::operator=(ShaderHandle&& other) noexcept
{
    // Note that swap is implemented with move internally
    std::swap(shaderID, other.shaderID);
    return *this;
}

ShaderHandle::~ShaderHandle()
{
    glDeleteShader(shaderID);
}

ShaderHandle::operator GLuint() const
{
    return shaderID;
}

GLuint ShaderHandle::Value() const
{
    return shaderID;
}

//------------------------------------------------------------------------------

ShaderProgramHandle::ShaderProgramHandle() : programID(glCreateProgram())
{
}

ShaderProgramHandle::ShaderProgramHandle(ShaderProgramHandle&& other) noexcept
    : programID(std::move(other.programID))
{
    other.programID = 0;
}

ShaderProgramHandle& ShaderProgramHandle::operator=(
    ShaderProgramHandle&& other) noexcept
{
    std::swap(programID, other.programID);
    return *this;
}

ShaderProgramHandle::~ShaderProgramHandle()
{
    glDeleteProgram(programID);
}

ShaderProgramHandle::operator GLuint() const
{
    return programID;
}

GLuint ShaderProgramHandle::Value() const
{
    return programID;
}

//------------------------------------------------------------------------------

VertexArrayHandle::VertexArrayHandle()
    : vaoID(0)  // Due to OpenGL syntax, we can't initial directly here, like we
                // want.
{
    glGenVertexArrays(1, &vaoID);
}

VertexArrayHandle::VertexArrayHandle(VertexArrayHandle&& other) noexcept
    : vaoID(std::move(other.vaoID))
{
    other.vaoID = 0;
}

VertexArrayHandle& VertexArrayHandle::operator=(
    VertexArrayHandle&& other) noexcept
{
    std::swap(vaoID, other.vaoID);
    return *this;
}

VertexArrayHandle::~VertexArrayHandle()
{
    glDeleteVertexArrays(1, &vaoID);
}

VertexArrayHandle::operator GLuint() const
{
    return vaoID;
}

GLuint VertexArrayHandle::Value() const
{
    return vaoID;
}

//------------------------------------------------------------------------------

BufferHandle::BufferHandle()
    : vboID(0)  // Due to OpenGL syntax, we can't initial directly here, like we
                // want.
{
    glGenBuffers(1, &vboID);
}

BufferHandle::BufferHandle(BufferHandle&& other) noexcept
    : vboID(std::move(other.vboID))
{
    other.vboID = 0;
}

BufferHandle& BufferHandle::operator=(
    BufferHandle&& other) noexcept
{
    std::swap(vboID, other.vboID);
    return *this;
}

BufferHandle::~BufferHandle()
{
    glDeleteBuffers(1, &vboID);
}

BufferHandle::operator GLuint() const
{
    return vboID;
}

GLuint BufferHandle::Value() const
{
    return vboID;
}

//------------------------------------------------------------------------------

TextureHandle::TextureHandle()
    : textureID(0)  // Due to OpenGL syntax, we can't initial directly here,
                    // like we want.
{
    glGenTextures(1, &textureID);
}

TextureHandle::TextureHandle(TextureHandle&& other) noexcept
    : textureID(std::move(other.textureID))
{
    other.textureID = 0;
}

TextureHandle& TextureHandle::operator=(TextureHandle&& other) noexcept
{
    std::swap(textureID, other.textureID);
    return *this;
}

TextureHandle::~TextureHandle()
{
    glDeleteTextures(1, &textureID);
}

TextureHandle::operator GLuint() const
{
    return textureID;
}

GLuint TextureHandle::Value() const
{
    return textureID;
}
