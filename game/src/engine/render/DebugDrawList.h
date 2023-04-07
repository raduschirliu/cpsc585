#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "engine/core/Colors.h"
#include "engine/core/gfx/Buffer.h"
#include "engine/core/gfx/VertexArray.h"
#include "engine/core/gfx/VertexBuffer.h"
#include "engine/core/math/Cuboid.h"

struct DebugVertex
{
    glm::vec3 pos;
    Color4u color;

    DebugVertex(const glm::vec3& pos, const Color4u& color)
        : pos(pos),
          color(color)
    {
    }

    DebugVertex(const glm::vec3& pos) : pos(pos), color(Color4u(255, 0, 0, 255))
    {
    }
};

class DebugDrawList
{
  public:
    DebugDrawList();

    void AddLine(const DebugVertex& start, const DebugVertex& end);
    void AddCuboid(const Cuboid& cuboid, const Color4u& color);

    void Prepare();
    void Draw();
    void Clear();

    bool HasItems() const;

  private:
    VertexArray vertex_array_;
    VertexBuffer vertex_buffer_;
    std::vector<DebugVertex> lines_;
};
