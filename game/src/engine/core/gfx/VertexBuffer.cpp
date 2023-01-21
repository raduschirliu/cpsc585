#include "engine/core/gfx/VertexBuffer.h"

#include <utility>

VertexBuffer::VertexBuffer() : Buffer<BufferType::kVertexArray>()
{
}

void VertexBuffer::ConfigureAttribute(GLuint index, GLint size,
                                      GLenum data_type, GLsizei stride,
                                      GLsizei offset)
{
    Bind();
    glVertexAttribPointer(index, size, data_type, GL_FALSE, stride,
                          reinterpret_cast<GLvoid*>(offset));
    glEnableVertexAttribArray(index);
}
