#pragma once

#include <vector>

#include "engine/core/gfx/Texture.h"
#include "engine/core/gfx/Vertex.h"

struct Mesh
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};
