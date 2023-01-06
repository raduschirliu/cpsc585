#include "engine/core/gfx/VertexBuffer.h"

#include <utility>

VertexBuffer::VertexBuffer() : Buffer<BufferType::kVertexArray>()
{
}

void VertexBuffer::ConfigureAttribute(GLuint index, GLint size,
                                      GLenum data_type)
{
    Bind();
    glVertexAttribPointer(index, size, data_type, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(index);
}
