#pragma once

#include "core/gfx/ShaderProgram.h"

class PhongLighting
{
  public:
    static void Apply(ShaderProgram& shader);
};