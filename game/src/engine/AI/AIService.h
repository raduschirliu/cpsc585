#pragma once

#include <glm/vec3.hpp>
#include <vector>

#include "engine/service/Service.h"

struct Faces
{
    glm::vec3 f_v1, f_v2, f_v3;
    glm::vec3 centroid;

    glm::vec3 CalculateCentroid() const;
    Faces(glm::vec3& f1, glm::vec3& f2, glm::vec3& f3);
};

class AIService final : public Service
{
  public:
    AIService();

    // From Service
    void OnInit() override;
    void OnStart(ServiceProvider& service_provider) override;
    void OnUpdate() override;
    void OnCleanup() override;
    std::string_view GetName() const override;
    void ReadVertices();

    std::vector<glm::vec3> GetPath();

  private:
    std::vector<glm::vec3> vertices_;
    std::vector<Faces> faces_;
    std::vector<glm::vec3> faces_index_;
    std::vector<glm::vec3> final_smooth_points_;
    std::vector<Faces> sorted_faces_;
};
