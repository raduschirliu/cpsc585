#pragma once

#include <GL/glew.h>

#include "engine/gfx/GLHandles.h"

class VertexArray
{
  public:
    VertexArray();

    void Bind() const;

  private:
    VertexArrayHandle handle_;
};
