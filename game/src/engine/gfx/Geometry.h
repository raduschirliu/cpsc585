#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>

#include "engine/gfx/ElementBuffer.h"
#include "engine/gfx/Vertex.h"
#include "engine/gfx/VertexArray.h"
#include "engine/gfx/VertexBuffer.h"

// VAO and two VBOs for storing vertices and colours, respectively
class Geometry
{
  public:
    enum class Type : GLenum
    {
        kLines = GL_LINES,
        kLineStrip = GL_LINE_STRIP,
        kTriangles = GL_TRIANGLES,
        kTriangleStrip = GL_TRIANGLE_STRIP,
    };

    Geometry(Type type);

    void Draw() const;
    void ClearVertices();
    void PushVertices(const std::vector<Vertex>& vertices);
    // Utility function for pushing a 2D array of vertices, which get unpacked
    // and pushed in-order
    void PushVertices(const std::vector<std::vector<Vertex>>& vertices);
    void PushIndices(const std::vector<uint32_t>& indices);
    void UploadData();
    void SetWireframe(bool state);
    void SetActive(bool state);
    void ComputeNormals();

    bool Active() const;
    const std::vector<Vertex>& Vertices() const;

  private:
    // note: due to how OpenGL works, vao needs  to be
    // defined and initialized before the vertex buffers
    VertexArray vertex_array_;

    VertexBuffer vertex_buffer_;
    VertexBuffer normal_buffer_;
    VertexBuffer color_buffer_;
    VertexBuffer uv_buffer_;
    ElementBuffer index_buffer_;

    bool active_;
    bool dirty_;
    bool wireframe_;
    size_t vertex_count_;
    Type type_;
    std::vector<Vertex> vertex_data_;
    std::vector<uint32_t> index_data_;

    void Bind() const;
    GLenum GetDrawType() const;
    void UploadIndexedVertices();
};
