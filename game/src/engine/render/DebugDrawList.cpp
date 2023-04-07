#include "engine/render/DebugDrawList.h"

DebugDrawList::DebugDrawList() : vertex_array_(), vertex_buffer_(), lines_{}
{
    const GLsizei stride = sizeof(DebugVertex);
    constexpr GLsizei pos_offset = offsetof(DebugVertex, pos);
    constexpr GLsizei color_offset = offsetof(DebugVertex, color);

    vertex_array_.Bind();
    vertex_buffer_.Bind();

    // Position
    vertex_buffer_.ConfigureAttribute(0, 3, GL_FLOAT, stride, pos_offset);
    // Color
    vertex_buffer_.ConfigureAttribute(1, 4, GL_UNSIGNED_BYTE, true, stride,
                                      color_offset);
}

void DebugDrawList::AddLine(const DebugVertex& start, const DebugVertex& end)
{
    lines_.insert(lines_.end(), {start, end});
}

void DebugDrawList::AddCuboid(const Cuboid& cuboid, const Color4u& color)
{
    // TODO(radu): This probably should not be drawn as lines, but as triangles
    // instead...
    lines_.insert(lines_.end(), {
                                    // Front face

                                    // Left side
                                    DebugVertex(cuboid.front.top_left, color),
                                    DebugVertex(cuboid.front.bot_left, color),
                                    // Bottom side
                                    DebugVertex(cuboid.front.bot_left, color),
                                    DebugVertex(cuboid.front.bot_right, color),
                                    // Right side
                                    DebugVertex(cuboid.front.bot_right, color),
                                    DebugVertex(cuboid.front.top_right, color),
                                    // Top side
                                    DebugVertex(cuboid.front.top_right, color),
                                    DebugVertex(cuboid.front.top_left, color),

                                    // Back face

                                    // Left side
                                    DebugVertex(cuboid.back.top_left, color),
                                    DebugVertex(cuboid.back.bot_left, color),
                                    // Bottom side
                                    DebugVertex(cuboid.back.bot_left, color),
                                    DebugVertex(cuboid.back.bot_right, color),
                                    // Right side
                                    DebugVertex(cuboid.back.bot_right, color),
                                    DebugVertex(cuboid.back.top_right, color),
                                    // Top side
                                    DebugVertex(cuboid.back.top_right, color),
                                    DebugVertex(cuboid.back.top_left, color),

                                    // Connecting
                                    DebugVertex(cuboid.front.top_left, color),
                                    DebugVertex(cuboid.back.top_left, color),
                                    DebugVertex(cuboid.front.top_right, color),
                                    DebugVertex(cuboid.back.top_right, color),
                                    DebugVertex(cuboid.front.bot_left, color),
                                    DebugVertex(cuboid.back.bot_left, color),
                                    DebugVertex(cuboid.front.bot_right, color),
                                    DebugVertex(cuboid.back.bot_right, color),
                                });
}

void DebugDrawList::Prepare()
{
    // TODO(radu): OPTIMIZATION - Would be better to allocate buffer at start
    // and only upload to part of the buffer instead
    vertex_buffer_.Upload(lines_, GL_DYNAMIC_DRAW);
}

void DebugDrawList::Draw()
{
    vertex_array_.Bind();

    const GLsizei line_vertex_count = lines_.size() * 2;
    glDrawArrays(GL_LINES, 0, line_vertex_count);
}

void DebugDrawList::Clear()
{
    lines_.clear();
}

bool DebugDrawList::HasItems() const
{
    return !lines_.empty();
}