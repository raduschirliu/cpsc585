#include "engine/core/gfx/VertexArray.h"

#include <utility>

void VertexArray::Unbind()
{
    glBindVertexArray(0);
}

VertexArray::VertexArray() : handle_{}
{
    Bind();
}

void VertexArray::Bind() const
{
    glBindVertexArray(handle_);
}