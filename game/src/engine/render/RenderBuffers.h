#pragma once

#include "engine/core/gfx/VertexArray.h"
#include "engine/core/gfx/VertexBuffer.h"

struct RenderBuffers
{
    VertexArray vertex_array;
    VertexBuffer vertex_buffer;
    ElementArrayBuffer element_buffer;
};
