#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "engine/core/Colors.h"
#include "engine/core/gfx/Buffer.h"
#include "engine/core/gfx/VertexArray.h"
#include "engine/core/gfx/VertexBuffer.h"

struct LineVertex
{
    glm::vec3 pos;
    Color4u color;

    LineVertex(const glm::vec3& pos, const Color4u& color)
        : pos(pos),
          color(color)
    {
    }

    LineVertex(const glm::vec3& pos) : pos(pos), color(Color4u(255, 0, 0, 255))
    {
    }
};

class DebugDrawList
{
  public:
    DebugDrawList();

    void AddLine(const LineVertex& start, const LineVertex& end);

    void Prepare();
    void Draw();
    void Clear();

    bool HasItems() const;

  private:
    VertexArray vertex_array_;
    VertexBuffer vertex_buffer_;
    std::vector<LineVertex> lines_;
};
