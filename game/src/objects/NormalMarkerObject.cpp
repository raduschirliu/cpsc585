#include "objects/NormalMarkerObject.h"

#include <limits>

#include "core/Utils.h"

using glm::vec3;
using std::make_shared;
using std::shared_ptr;
using std::vector;
using utils::Lerp;

static constexpr vec3 kNegColor = utils::colors::kRed;
static constexpr float kNormalLength = 10.0f;

NormalMarkerObject::NormalMarkerObject(GameObject& target_obj)
    : GameObject(),
      target_geometry_(target_obj.Geometry()),
      color_(utils::colors::kCyan)
{
    ASSERT_MSG(target_geometry_, "Target does not have valid geometry");
    BuildGeometry();
}

void NormalMarkerObject::BuildGeometry()
{
    vector<Vertex> vertices;
    vector<uint32_t> indices;
    uint32_t next_index = 0;

    auto geometry = make_shared<::Geometry>(Geometry::Type::kLines);
    SetGeometry(geometry);

    for (const auto& vertex : target_geometry_->Vertices())
    {
        vec3 start_pos = vertex.position;
        vec3 end_pos = vertex.position + vertex.normal * kNormalLength;

        vec3 color = utils::Map(vertex.normal, vec3(-1.0f, -1.0f, -1.0f),
                                vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, 0.0f),
                                vec3(1.0f, 1.0f, 1.0f));

        vertices.push_back(Vertex(start_pos, color));
        vertices.push_back(Vertex(end_pos, color));

        indices.push_back(next_index);
        indices.push_back(next_index + 1);
        next_index += 2;
    }

    geometry->PushVertices(vertices);
    geometry->PushIndices(indices);
    geometry->UploadData();
}