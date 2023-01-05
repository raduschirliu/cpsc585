#pragma once

#include "engine/gfx/Buffer.h"

class VertexBuffer final : public Buffer<BufferType::kVertexArray>
{
  public:
    VertexBuffer();

    void ConfigureAttribute(GLuint index, GLint size, GLenum data_type);
};
