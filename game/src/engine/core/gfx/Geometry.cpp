#include "engine/core/gfx/Geometry.h"

#include <glm/gtx/hash.hpp>
#include <utility>

#include "engine/core/debug/Assert.h"
#include "engine/core/debug/Log.h"

using glm::vec2;
using glm::vec3;
using std::make_pair;
using std::pair;
using std::shared_ptr;
using std::unordered_map;
using std::vector;

Geometry::Geometry(Geometry::Type type)
    : vertex_array_(),
      vertex_buffer_(),
      normal_buffer_(),
      color_buffer_(),
      uv_buffer_(),
      index_buffer_(),
      active_(true),
      dirty_(false),
      wireframe_(false),
      vertex_count_(0),
      type_(type),
      vertex_data_{},
      index_data_{}
{
    vertex_buffer_.ConfigureAttribute(0, 3, GL_FLOAT);
    normal_buffer_.ConfigureAttribute(1, 3, GL_FLOAT);
    color_buffer_.ConfigureAttribute(2, 3, GL_FLOAT);
    uv_buffer_.ConfigureAttribute(3, 2, GL_FLOAT);
}

void Geometry::Draw() const
{
    if (!active_)
    {
        return;
    }

    ASSERT_MSG(!dirty_, "Drawing dirty geometry");

    ASSERT_MSG((type_ != Type::kLines) ||
                   (type_ == Type::kLines && vertex_count_ % 2 == 0),
               "Each line needs 2 vertices to be valid");

    ASSERT_MSG((type_ != Type::kLineStrip) ||
                   (type_ == Type::kLineStrip && vertex_count_ != 1),
               "Either need vertices >= 2 or vertices == 0");

    ASSERT_MSG((type_ != Type::kTriangles) ||
                   (type_ == Type::kTriangles && vertex_count_ % 3 == 0),
               "Each triangle needs 3 vertices");

    if (wireframe_)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (vertex_count_ > 0)
    {
        Bind();
        glDrawElements(GetDrawType(), static_cast<GLsizei>(vertex_count_),
                       GL_UNSIGNED_INT, nullptr);
    }
}

void Geometry::ClearVertices()
{
    dirty_ = true;
    vertex_count_ = 0;
    vertex_data_.clear();
    index_data_.clear();
}

void Geometry::PushVertices(const vector<Vertex>& vertices)
{
    vertex_data_.insert(vertex_data_.end(), vertices.begin(), vertices.end());
    dirty_ = true;
}

void Geometry::PushVertices(const vector<vector<Vertex>>& vertices)
{
    for (const auto& row : vertices)
    {
        vertex_data_.insert(vertex_data_.end(), row.begin(), row.end());
    }

    dirty_ = true;
}

void Geometry::PushIndices(const vector<uint32_t>& indices)
{
    index_data_.insert(index_data_.end(), indices.begin(), indices.end());
    dirty_ = true;
}

void Geometry::UploadData()
{
    if (!dirty_)
    {
        return;
    }

    Bind();
    UploadIndexedVertices();

    dirty_ = false;
}

void Geometry::SetWireframe(bool state)
{
    wireframe_ = state;
}

void Geometry::SetActive(bool state)
{
    active_ = state;
}

void Geometry::ComputeNormals()
{
    ASSERT_MSG(type_ == Type::kTriangles,
               "Cant compute normals for non-triangular geometry");
    ASSERT_MSG(vertex_data_.size() > 0,
               "Need at least 1 vertex to compute normals");
    ASSERT_MSG(index_data_.size() >= 3,
               "Need at least 1 face to compute normals");

    // Find normals
    for (size_t i = 0; i + 3 < index_data_.size(); i += 3)
    {
        // For each face
        size_t a = index_data_[i];
        size_t b = index_data_[i + 1];
        size_t c = index_data_[i + 2];

        Vertex& va = vertex_data_[a];
        Vertex& vb = vertex_data_[b];
        Vertex& vc = vertex_data_[c];

        const vec3 d_ab = vb.position - va.position;
        const vec3 d_ac = vc.position - va.position;
        const vec3 normal = glm::cross(d_ab, d_ac);

        va.normal += normal;
        vb.normal += normal;
        vc.normal += normal;
    }

    // Normalize
    for (auto& vertex : vertex_data_)
    {
        if (glm::length(vertex.normal) > 0.0f)
        {
            vertex.normal = glm::normalize(vertex.normal);
        }
    }

    dirty_ = true;
}

bool Geometry::Active() const
{
    return active_;
}

const vector<Vertex>& Geometry::Vertices() const
{
    return vertex_data_;
}

void Geometry::Bind() const
{
    vertex_array_.Bind();
}

GLenum Geometry::GetDrawType() const
{
    return static_cast<GLenum>(type_);
}

void Geometry::UploadIndexedVertices()
{
    vector<vec3> vertices;
    vector<vec3> normals;
    vector<vec3> colors;
    vector<vec2> uvs;

    // TODO(radu): Re-work this to use a single buffer instead of this hell
    for (const auto& vertex : vertex_data_)
    {
        vertices.push_back(vertex.position);
        normals.push_back(vertex.normal);
        colors.push_back(vertex.color);
        uvs.push_back(vertex.uv);
    }

    vertex_buffer_.Upload(vertices, GL_STATIC_DRAW);
    normal_buffer_.Upload(normals, GL_STATIC_DRAW);
    color_buffer_.Upload(colors, GL_STATIC_DRAW);
    uv_buffer_.Upload(uvs, GL_STATIC_DRAW);
    index_buffer_.Upload(index_data_, GL_STATIC_DRAW);

    vertex_count_ = index_data_.size();
}