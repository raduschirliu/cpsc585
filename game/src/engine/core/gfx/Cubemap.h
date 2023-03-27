#pragma once

#include <string>

#include "engine/core/gfx/GLHandles.h"

class Cubemap
{
  public:
    Cubemap();

    void LoadTexture(uint32_t target, const std::string& path);
    void Finalize();
    void Bind(uint32_t slot = 0) const;

  private:
    TextureHandle handle_;
};
