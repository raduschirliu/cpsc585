#include "engine/core/gfx/ShaderProgram.h"

#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "engine/core/debug/Assert.h"
#include "engine/core/debug/Log.h"

using glm::mat4;
using glm::value_ptr;
using glm::vec3;
using std::string;

ShaderProgram::ShaderProgram(const string& vertexPath,
                             const string& fragmentPath)
    : programID(),
      vertex(vertexPath, GL_VERTEX_SHADER),
      fragment(fragmentPath, GL_FRAGMENT_SHADER)
{
    attach(*this, vertex);
    attach(*this, fragment);
    glLinkProgram(programID);

    if (!CheckAndLogLinkSuccess())
    {
        glDeleteProgram(programID);
        throw std::runtime_error("Shaders did not link.");
    }
}

bool ShaderProgram::Recompile()
{
    try
    {
        ShaderProgram new_program(vertex.getPath(), fragment.getPath());
        *this = std::move(new_program);
        return true;
    }
    catch (std::runtime_error& e)
    {
        debug::LogWarn(
            "SHADER_PROGRAM falling back to previous version of shaders");
        UNUSED(e);
        return false;
    }
}

void ShaderProgram::SetUniform(const string& name, mat4 value)
{
    GLint location = GetUniformLocation(name);
    ASSERT_MSG(location >= 0, "Uniform must exist");

    glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(value));
}

void ShaderProgram::SetUniform(const std::string& name, vec3 value)
{
    GLint location = GetUniformLocation(name);
    ASSERT_MSG(location >= 0, "Uniform must exist");

    glUniform3fv(location, 1, glm::value_ptr(value));
}

void ShaderProgram::SetUniform(const std::string& name, float value)
{
    GLint location = GetUniformLocation(name);
    ASSERT_MSG(location >= 0, "Uniform must exist");

    glUniform1f(location, value);
}

void ShaderProgram::SetUniform(const std::string& name, bool value)
{
    GLint location = GetUniformLocation(name);
    ASSERT_MSG(location >= 0, "Uniform must exist");

    glUniform1i(location, value ? 1 : 0);
}

void ShaderProgram::SetUniform(const std::string& name, int value)
{
    GLint location = GetUniformLocation(name);
    ASSERT_MSG(location >= 0, "Uniform must exist");

    glUniform1i(location, value);
}

void attach(ShaderProgram& sp, Shader& s)
{
    glAttachShader(sp.programID, s.shaderID);
}

bool ShaderProgram::CheckAndLogLinkSuccess() const
{
    GLint success;

    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success)
    {
        GLint logLength;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> log(logLength);
        glGetProgramInfoLog(programID, logLength, NULL, log.data());

        debug::LogError("SHADER_PROGRAM linking {} + {}:\n{}", vertex.getPath(),
                        fragment.getPath(), log.data());
        return false;
    }
    else
    {
        debug::LogInfo(
            "SHADER_PROGRAM successfully compiled and linked {} + {}",
            vertex.getPath(), fragment.getPath());
        return true;
    }
}

void ShaderProgram::Use() const
{
    glUseProgram(programID);
}

GLint ShaderProgram::GetUniformLocation(const string& name) const
{
    return glGetUniformLocation(programID, name.c_str());
}
