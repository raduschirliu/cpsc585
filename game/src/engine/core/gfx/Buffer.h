#pragma once

#include <GL/glew.h>

#include <vector>

#include "engine/core/debug/Assert.h"
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
    Buffer()
        : handle_(),
          type_(static_cast<GLenum>(Type)),
          usage_(GL_STATIC_DRAW),
          size_(0)
    {
    }

    void Bind() const
    {
        glBindBuffer(type_, handle_);
    }

    void Allocate(size_t size, GLenum usage)
    {
        size_ = static_cast<GLsizeiptr>(size);
        usage_ = usage;

        Bind();
        glBufferData(type_, size, nullptr, usage_);
    }

    /**
     * If the buffer is currently too small to fit the given data, re-allocate
     * an empty buffer
     */
    template <class T>
    void ResizeToFit(const std::vector<T>& data)
    {
        const GLsizeiptr data_size =
            static_cast<GLsizeiptr>(sizeof(T) * data.size());

        if (size_ < data_size)
        {
            Allocate(data_size, usage_);
        }
    }

    template <class T>
    void UploadSubset(const std::vector<T>& data, size_t offset)
    {
        const GLsizeiptr data_offset = static_cast<GLintptr>(offset);
        const GLsizeiptr data_size =
            static_cast<GLsizeiptr>(sizeof(T) * data.size());

        ASSERT_MSG(data_offset >= 0 && (data_offset + data_size) <= size_,
                   "Data subset must be within buffer bounds");

        Bind();
        glBufferSubData(type_, data_offset, data_size, data.data());
    }

    template <class T>
    void Upload(const std::vector<T>& data, GLenum usage)
    {
        size_ = static_cast<GLsizeiptr>(sizeof(T) * data.size());
        usage_ = usage;

        Bind();
        glBufferData(type_, size_, data.data(), usage_);
    }

    void Upload(const void* data, size_t size, GLenum usage)
    {
        size_ = static_cast<GLsizeiptr>(size);
        usage_ = usage;

        Bind();
        glBufferData(type_, size_, data, usage_);
    }

  private:
    BufferHandle handle_;
    GLenum type_;
    GLenum usage_;
    GLsizeiptr size_;
};

using ElementArrayBuffer = Buffer<BufferType::kElementArray>;
