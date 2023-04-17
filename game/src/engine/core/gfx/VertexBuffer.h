#pragma once

#include "engine/core/gfx/Buffer.h"

class VertexBuffer final : public Buffer<BufferType::kVertexArray>
{
  public:
    VertexBuffer();

    void ConfigureIntAttribute(GLuint index, GLint size, GLenum data_type,
                               GLsizei stride, GLsizei offset);
    void ConfigureAttribute(GLuint index, GLint size, GLenum data_type,
                            GLsizei stride, GLsizei offset);
    void ConfigureAttribute(GLuint index, GLint size, GLenum data_type,
                            bool normalize, GLsizei stride, GLsizei offset);

    void AttributeDivisor(GLuint index, GLuint divisor);
};
