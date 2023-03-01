#include "engine/render/DebugDrawList.h"

DebugDrawList::DebugDrawList() : vertex_array_(), vertex_buffer_(), lines_{}
{
    const GLsizei stride = sizeof(LineVertex);
    constexpr GLsizei pos_offset = offsetof(LineVertex, pos);
    constexpr GLsizei color_offset = offsetof(LineVertex, color);

    vertex_array_.Bind();
    vertex_buffer_.Bind();

    // Position
    vertex_buffer_.ConfigureAttribute(0, 3, GL_FLOAT, stride, pos_offset);
    // Color
    vertex_buffer_.ConfigureAttribute(1, 4, GL_UNSIGNED_BYTE, true, stride,
                                      color_offset);
}

void DebugDrawList::AddLine(const LineVertex& start, const LineVertex& end)
{
    lines_.insert(lines_.end(), {start, end});
}

void DebugDrawList::Prepare()
{
    // OPTIMIZATION: Would be better to allocate buffer at start and only upload
    // to part of the buffer instead
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