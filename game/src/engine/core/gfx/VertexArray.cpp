#include "engine/core/gfx/VertexArray.h"

#include <utility>

VertexArray::VertexArray() : handle_{}
{
    Bind();
}

void VertexArray::Bind() const
{
    glBindVertexArray(handle_);
}