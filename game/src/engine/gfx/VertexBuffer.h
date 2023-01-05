#pragma once

#include <GL/glew.h>

#include <vector>

#include "engine/gfx/GLHandles.h"

class VertexBuffer
{
  public:
    VertexBuffer(GLuint index, GLint size, GLenum data_type);

    // A vertex buffer with multiple attribute pointers for matrices
    VertexBuffer(GLuint start_index, GLuint matrix_columns, GLsizei column_size,
                 GLenum data_type);

    // Because we're using the VertexBufferHandle to do RAII for the buffer for
    // us and our other types are trivial or provide their own RAII we don't
    // have to provide any specialized functions here. Rule of zero
    //
    // https://en.cppreference.com/w/cpp/language/rule_of_three
    // https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#Rc-zero

    void Bind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, buffer_id_);
    }

    void Upload(GLsizeiptr size, const void* data, GLenum usage);

    template <class T>
    void Upload(const std::vector<T>& data, GLenum usage)
    {
        Bind();
        glBufferData(GL_ARRAY_BUFFER, sizeof(T) * data.size(), data.data(),
                     usage);
    }

  private:
    VertexBufferHandle buffer_id_;
};
