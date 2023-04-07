#pragma once

#include <glm/glm.hpp>

struct Cuboid
{
    struct Face
    {
        glm::vec3 top_left;
        glm::vec3 bot_left;
        glm::vec3 bot_right;
        glm::vec3 top_right;
    };

    Face front;
    Face back;

    void BoundsFromNdcs(const glm::mat4& proj_view);
    glm::vec3* GetVertexList();
    glm::vec3 GetCentroid() const;
    glm::vec3 GetCentroidMidpoint(float t) const;
    const glm::vec3* GetVertexList() const;

    constexpr size_t GetVertexCount() const
    {
        // 2 faces, 4 vertices each
        return 2 * 4;
    }
};
