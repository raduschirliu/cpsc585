#include "engine/core/gfx/Shader.h"

#include <string.h>

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "engine/core/debug/Assert.h"
#include "engine/core/debug/Log.h"

// Of course GCC and MSVC don't both have the same safe strerror func... :(
#if defined(_WIN32) || defined(_WIN64)
#define strerror_r(errno, buf, len) strerror_s(buf, len, errno)
#endif

static constexpr size_t kErrorBufferSize = 2048;

Shader::Shader(const std::string& path, GLenum type)
    : shaderID(type),
      type(type),
      path(path)
{
    if (!compile())
    {
        throw std::runtime_error("Shader did not compile");
    }
}

bool Shader::compile()
{
    // read shader source
    std::string sourceString;
    std::ifstream file;

    // ensure ifstream objects can throw exceptions:
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open file
        file.open(path);
        std::stringstream sourceStream;

        // read file buffer contents into stream
        sourceStream << file.rdbuf();

        // close file handler
        file.close();

        // convert stream into string
        sourceString = sourceStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        UNUSED(e);

        char error_str[kErrorBufferSize];
        strerror_r(errno, error_str, kErrorBufferSize);

        Log::error("SHADER reading {}:\n{}", path, error_str);
        return false;
    }
    const GLchar* sourceCode = sourceString.c_str();

    // compile shader
    glShaderSource(shaderID, 1, &sourceCode, NULL);
    glCompileShader(shaderID);

    // check for errors
    GLint success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        GLint logLength;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> log(logLength);
        glGetShaderInfoLog(shaderID, logLength, NULL, log.data());

        Log::error("SHADER compiling {}:\n{}", path, log.data());
    }
    return success;
}
