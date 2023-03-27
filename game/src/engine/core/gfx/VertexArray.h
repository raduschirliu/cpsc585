#pragma once

#include <GL/glew.h>

#include "engine/core/gfx/GLHandles.h"

class VertexArray
{
  public:
    static void Unbind();

    VertexArray();

    void Bind() const;

  private:
    VertexArrayHandle handle_;
};
