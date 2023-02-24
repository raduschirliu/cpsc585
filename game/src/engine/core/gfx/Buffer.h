#pragma once

#include <GL/glew.h>

#include <vector>

#include "engine/core/gfx/GLHandles.h"

enum class BufferType : GLenum
{
    kElementArray = GL_ELEMENT_ARRAY_BUFFER,
    kVertexArray = GL_ARRAY_BUFFER
};

template <BufferType Type>
class Buffer
{
  public:
    Buffer() : handle_(), type_(static_cast<GLenum>(Type))
    {
    }

    void Bind() const
    {
        glBindBuffer(type_, handle_);
    }

    template <class T>
    void Upload(const std::vector<T>& data, GLenum usage)
    {
        GLsizei size = static_cast<GLsizei>(sizeof(T) * data.size());

        Bind();
        glBufferData(type_, size, data.data(), usage);
    }

    void Upload(const void* data, size_t size, GLenum usage)
    {
        Bind();
        glBufferData(type_, size, data, usage);
    }

  private:
    BufferHandle handle_;
    GLenum type_;
};

using ElementArrayBuffer = Buffer<BufferType::kElementArray>;
