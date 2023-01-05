#include "engine/gfx/VertexArray.h"

#include <utility>

VertexArray::VertexArray() : handle_{}
{
    Bind();
}

void VertexArray::Bind() const
{
    glBindVertexArray(handle_);
}