#pragma once

#include <vector>

#include "engine/core/gfx/Vertex.h"

struct Mesh
{
    std::string name;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};
