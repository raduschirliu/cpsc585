#include "core/gfx/ElementBuffer.h"

#include <utility>

ElementBuffer::ElementBuffer() : buffer_id_{}
{
    Bind();
}

void ElementBuffer::Bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_id_);
}

void ElementBuffer::Upload(const std::vector<uint32_t>& indices, GLenum usage)
{
    GLsizei size = static_cast<GLsizei>(sizeof(uint32_t) * indices.size());

    Bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices.data(), usage);
}
