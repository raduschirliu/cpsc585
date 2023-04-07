#include "engine/core/math/Cuboid.h"

using glm::mat4;
using glm::vec3;
using glm::vec4;

static constexpr Cuboid kDefaultFrustumNdc{
    // "Near" face
    .front{
        .top_left = vec3(-1.0f, 1.0f, -1.0f),
        .bot_left = vec3(-1.0f, -1.0f, -1.0f),
        .bot_right = vec3(1.0f, -1.0f, -1.0f),
        .top_right = vec3(1.0f, 1.0f, -1.0f),
    },
    // "Far" face
    .back{
        .top_left = vec3(-1.0f, 1.0f, 1.0f),
        .bot_left = vec3(-1.0f, -1.0f, 1.0f),
        .bot_right = vec3(1.0f, -1.0f, 1.0f),
        .top_right = vec3(1.0f, 1.0f, 1.0f),
    },
};

void Cuboid::BoundsFromNdcs(const mat4& proj_view)
{
    mat4 inverse_proj_view = glm::inverse(proj_view);
    const vec3* default_vertex = kDefaultFrustumNdc.GetVertexList();
    vec3* vertex = GetVertexList();
    vec4 new_vert;

    for (size_t i = 0; i < GetVertexCount(); i++)
    {
        new_vert = inverse_proj_view * vec4(*default_vertex, 1.0f);
        *vertex = vec3(new_vert / new_vert.w);

        default_vertex++;
        vertex++;
    }
}

vec3 Cuboid::GetCentroid() const
{
    return GetCentroidMidpoint(0.5f);
}

vec3 Cuboid::GetCentroidMidpoint(float t) const
{
    vec3 front_centroid(0.0f);
    vec3 back_centroid(0.0f);
    const vec3* vertex = GetVertexList();

    for (size_t i = 0; i < GetVertexCount(); i++)
    {
        if (i >= 4)
        {
            back_centroid += *vertex;
        }
        else
        {
            front_centroid += *vertex;
        }

        vertex++;
    }

    front_centroid /= 4.0f;
    back_centroid /= 4.0f;
    return glm::mix(front_centroid, back_centroid, t);
}

vec3* Cuboid::GetVertexList()
{
    return &front.top_left;
}

const vec3* Cuboid::GetVertexList() const
{
    return &front.top_left;
}