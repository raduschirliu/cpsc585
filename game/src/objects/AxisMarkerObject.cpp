#include "objects/AxisMarkerObject.h"

#include <limits>

#include "core/Utils.h"

using glm::vec3;
using std::make_shared;
using std::shared_ptr;
using utils::Lerp;

static constexpr float kLerpStep = 0.01f;

AxisMarkerObject::AxisMarkerObject(const vec3& axis, bool centered,
                                   float length, const vec3& color)
    : GameObject(),
      length_(length),
      centered_(centered),
      axis_(axis),
      color_(color)
{
    BuildGeometry();
}

void AxisMarkerObject::BuildGeometry()
{
    const vec3 axis_normalized = glm::normalize(axis_);
    vec3 start_pos(0.0f, 0.0f, 0.0f);
    vec3 end_pos = axis_normalized * length_;

    if (centered_)
    {
        const vec3 delta = axis_normalized * (length_ / 2.0f);
        start_pos -= delta;
        end_pos -= delta;
    }

    auto geometry = make_shared<::Geometry>(Geometry::Type::kLineStrip);
    SetGeometry(geometry);

    uint32_t index = 0;

    geometry->PushVertices({Vertex(start_pos, color_)});
    geometry->PushIndices({index});

    index++;

    for (float t = 0.0f; t <= 1.0f; t += kLerpStep)
    {
        geometry->PushVertices({Vertex(Lerp(start_pos, end_pos, t), color_)});
        geometry->PushIndices({index});

        index++;
    }

    geometry->PushVertices({Vertex(end_pos, color_)});
    geometry->PushIndices({index});

    geometry->UploadData();
}