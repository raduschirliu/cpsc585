#pragma once

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <map>
#include <set>
#include <stack>
#include <vector>

#include "../service/Service.h"

// struct Vertices
// {
//   glm::vec3 v1;
//   glm::vec3 v2;
//   glm::vec3 v3;

//   Vertices(glm::vec3& v1, glm::vec3& v2, glm::vec3& v3)
//   {
//     this->v1 = v1;
//     this->v2 = v2;
//     this->v3 = v3;
//   }
// };

struct Faces
{
    glm::vec3 f_v1, f_v2, f_v3;
    glm::vec3 centroid;

    glm::vec3 CalculateCentroid()
    {
        return (this->f_v1 + this->f_v2 + this->f_v3) / 3.f;
    }

    Faces(glm::vec3& f1, glm::vec3& f2, glm::vec3& f3)
    {
        this->f_v1 = f1;
        this->f_v2 = f2;
        this->f_v3 = f3;
        this->centroid = CalculateCentroid();
    }
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
