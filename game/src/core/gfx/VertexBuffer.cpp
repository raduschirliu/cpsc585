#include "core/gfx/VertexBuffer.h"

#include <utility>

VertexBuffer::VertexBuffer(GLuint index, GLint size, GLenum data_type)
    : buffer_id_{}
{
    Bind();
    glVertexAttribPointer(index, size, data_type, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(index);
}

VertexBuffer::VertexBuffer(GLuint start_index, GLuint matrix_columns,
                           GLsizei column_size, GLenum data_type)
    : buffer_id_{}
{
    Bind();
    GLsizei stride = column_size * matrix_columns;

    for (GLuint i = 0; i < matrix_columns; i++)
    {
        GLuint index = start_index + i;
        void* offset =
            reinterpret_cast<void*>(i * static_cast<GLuint>(column_size));

        glVertexAttribPointer(index, matrix_columns, data_type, GL_FALSE,
                              stride, offset);
        glEnableVertexAttribArray(index);
        glVertexAttribDivisor(index, 1);
    }
}

void VertexBuffer::Upload(GLsizeiptr size, const void* data, GLenum usage)
{
    Bind();
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
}
