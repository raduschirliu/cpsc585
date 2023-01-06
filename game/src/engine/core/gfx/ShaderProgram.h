#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <string>

#include "engine/core/gfx/GLHandles.h"
#include "engine/core/gfx/Shader.h"

class ShaderProgram
{
  public:
    ShaderProgram(const std::string& vertexPath,
                  const std::string& fragmentPath);

    // Because we're using the ShaderProgramHandle to do RAII for the shader for
    // us and our other types are trivial or provide their own RAII we don't
    // have to provide any specialized functions here. Rule of zero
    //
    // https://en.cppreference.com/w/cpp/language/rule_of_three
    // https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#Rc-zero

    bool Recompile();
    void SetUniform(const std::string& name, glm::mat4 value);
    void SetUniform(const std::string& name, glm::vec3 value);
    void SetUniform(const std::string& name, float value);
    void SetUniform(const std::string& name, bool value);
    void SetUniform(const std::string& name, int value);

    void Use() const
    {
        glUseProgram(programID);
    }

    void friend attach(ShaderProgram& sp, Shader& s);

  private:
    ShaderProgramHandle programID;

    Shader vertex;
    Shader fragment;

    bool CheckAndLogLinkSuccess() const;
    GLint GetUniformLocation(const std::string& name) const;
};
