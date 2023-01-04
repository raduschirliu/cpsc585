#include "objects/PlanetObject.h"

#include <glm/gtc/epsilon.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "core/Utils.h"
#include "core/gfx/Log.h"
#include "core/gfx/Texture.h"
#include "objects/NormalMarkerObject.h"

using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using std::make_shared;
using std::shared_ptr;
using std::vector;
using utils::kIdentityMatrix;
using utils::Lerp;

namespace colors = utils::colors;

static constexpr float kThetaMax = glm::pi<float>();
static constexpr float kPhiMax = glm::two_pi<float>();
static constexpr float kAxisMarkerExtraLength = 40.0f;

static constexpr size_t kThetaIterations = 20;
static constexpr size_t kPhiIterations = 40;

PlanetObject::PlanetObject(float radius)
    : GameObject(),
      rotation_transform_(),
      radius_(radius),
      rotation_axis_(0.0f, 1.0f, 0.0f, 0.0f),
      rotation_speed_rad_(0.0f),
      orbital_speed_rad_(0.0f),
      orbital_inclination_height_max_(0.0f),
      orbital_radius_(0.0f),
      orbit_time_(0.0f),
      orbit_target_(nullptr),
      axis_marker_obj_(make_shared<AxisMarkerObject>(
          vec3(0.0f, 1.0f, 0.0f), true, radius_ * 2.0f + kAxisMarkerExtraLength,
          colors::kMagenta))
{
    AddChild(axis_marker_obj_);
    BuildGeometry();
}

void PlanetObject::Update(float delta_time)
{
    GameObject::Update(delta_time);

    float time_scale = delta_time * utils::kTimeScale;

    // Orbit revolution around parent
    if (orbit_target_)
    {
        orbit_time_ += time_scale * orbital_speed_rad_;

        if (orbit_time_ > glm::two_pi<float>())
        {
            orbit_time_ -= glm::two_pi<float>();
        }

        float a = orbital_radius_;
        float u = 1.0f;
        float x = a * glm::cosh(u) * glm::cos(orbit_time_);
        float y = orbital_inclination_height_max_ * glm::sin(orbit_time_);
        float z = a * glm::sinh(u) * glm::sin(orbit_time_);

        vec3 pos(x, y, z);

        if (orbit_target_)
        {
            pos += orbit_target_->Transform()->GetPosition();
        }

        Transform()->SetPosition(pos);
    }

    // Rotation around planet's own axis
    if (rotation_speed_rad_ > 0.0f)
    {
        float delta = time_scale * rotation_speed_rad_;
        Transform()->Rotate(delta, vec3(0.0f, 1.0f, 0.0f));
    }
}

void PlanetObject::SetAxisMarkerVisible(bool visible)
{
    axis_marker_obj_->SetActive(visible);
}

void PlanetObject::SetWireframe(bool enabled)
{
    Geometry()->SetWireframe(enabled);
}

void PlanetObject::SetAxialRotation(float axial_tilt_rad,
                                    float rotation_speed_rad)
{
    rotation_axis_ =
        glm::rotate(kIdentityMatrix, axial_tilt_rad, vec3(1.0f, 0.0f, 0.0f)) *
        vec4(0.0f, 1.0f, 0.0f, 0.0f);
    rotation_speed_rad_ = rotation_speed_rad;

    Transform()->Rotate(axial_tilt_rad, vec3(1.0f, 0.0f, 0.0f));
}

void PlanetObject::SetOrbitalRevolution(shared_ptr<PlanetObject> orbit_target,
                                        float orbital_radius,
                                        float orbital_inclination_rad,
                                        float orbital_speed_rad)
{
    orbit_target_ = orbit_target;
    orbital_radius_ = orbital_radius;
    orbital_speed_rad_ = orbital_speed_rad;
    orbital_inclination_height_max_ =
        orbital_radius * glm::sin(orbital_inclination_rad);
}

void PlanetObject::Reset()
{
    orbit_time_ = 0;
}

mat4 PlanetObject::BuildModelMatrix()
{
    return Transform()->GetLocalToWorldMatrix();
}

void PlanetObject::BuildGeometry()
{
    const vec3 color = colors::kWhite;

    // Build vertices
    vector<vector<Vertex>> vertices;
    size_t num_vertices = 0;
    size_t num_rows = 0;
    size_t num_cols = 0;

    // Want to include both min/max theta/phi to avoid wonky texture
    // interpolating between first/last quads in each row
    for (size_t i_theta = 0; i_theta < kThetaIterations; i_theta++)
    {
        float v = static_cast<float>(i_theta) /
                  static_cast<float>(kThetaIterations - 1);
        float theta = (1.0f - v) * kThetaMax;
        num_cols = 0;
        vector<Vertex> row;

        // Phi loop is NOT inclusive of bound, as after 360deg we loop
        for (size_t i_phi = 0; i_phi < kPhiIterations; i_phi++)
        {
            constexpr float u_max = 1.01f;
            float u = utils::Map(static_cast<float>(i_phi), 0.0f,
                                 static_cast<float>(kPhiIterations - 1), 0.01f,
                                 u_max);
            float phi = (u_max - u) * kPhiMax;

            vec3 pos = utils::SphericalToCartesian(radius_, phi, theta);
            vec2 uv(u, v);

            // Since this is a sphere, the normal = the direction from origin ->
            // vertex pos
            vec3 normal = glm::normalize(pos);
            row.push_back(Vertex(pos, normal, color, uv));

            num_vertices++;
            num_cols++;
        }

        vertices.push_back(row);
        num_rows++;
    }

    // Determine indices to make faces
    vector<uint32_t> indices;

    for (uint32_t row = 0; row + 1 < num_rows; row++)
    {
        for (uint32_t col = 0; col < num_cols; col++)
        {
            // (row, col) = (0, 0) in the bottom left of the vertex grid
            uint32_t bottom_left = GetIndex(num_cols, row, col);
            uint32_t bottom_right = GetIndex(num_cols, row, col + 1);
            uint32_t top_left = GetIndex(num_cols, row + 1, col);
            uint32_t top_right = GetIndex(num_cols, row + 1, col + 1);

            ASSERT_MSG(top_left < num_vertices, "invalid idx");
            ASSERT_MSG(top_right < num_vertices, "invalid idx");
            ASSERT_MSG(bottom_left < num_vertices, "invalid idx");
            ASSERT_MSG(bottom_right < num_vertices, "invalid idx");

            // Build quads (2 triangles) with vertices in CCW order
            indices.insert(
                indices.end(),
                {
                    top_left, bottom_left, top_right,     // Left triangle
                    top_right, bottom_left, bottom_right  // Right triangle
                });
        }
    }

    auto geometry = make_shared<::Geometry>(Geometry::Type::kTriangles);
    SetGeometry(geometry);

    geometry->PushVertices(vertices);
    geometry->PushIndices(indices);
    // geometry->ComputeNormals();

    geometry->UploadData();
}

uint32_t PlanetObject::GetIndex(size_t num_cols, uint32_t row,
                                uint32_t col) const
{
    uint32_t col_wrapped = col % num_cols;
    return row * num_cols + col_wrapped;
}