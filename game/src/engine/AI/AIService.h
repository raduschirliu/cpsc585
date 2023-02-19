#pragma once

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <map>
#include <stack>
#include <vector>

#include "../service/Service.h"

//----------------
// Node
//----------------

class Node
{
  public:
    Node(unsigned int id, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2);
    ~Node();

    glm::vec3 Get_Centroid(Node* node);

    unsigned int id_;                    // used to define every triangle
    glm::vec3 v0_, v1_, v2_, centroid_;  // to define every point we need for
                                         // our navmesh algorithm to work

    std::vector<std::pair<float, Node*>>* connections_;

  private:
};

// -----------------------------
//          NAV MESH
// -----------------------------

using glm::vec3;

// Creating it here just for now
class NavMesh
{
  public:
    std::map<unsigned int, Node*>* nodes_;

    NavMesh();
    ~NavMesh();

    Node* FindEntity(glm::vec3 pos);

  private:
    float cost(Node* src, Node* dest);
};

//-----------------
// Pathfinder
//-----------------

class Pathfinder
{
  public:
    std::stack<vec3>* path_;
    NavMesh* navMesh_;

    Pathfinder(NavMesh* navMesh);

    bool Search(Node* src, Node* dest);
    vec3 GetNextWaypoint();
    bool PathEmpty();

  private:
    bool IsDestination(Node* src, Node* dest);
    float CalculateHCost(Node* src, Node* dest);
    void TracePath(Node* src, Node* dest,
                   std::map<unsigned int, unsigned int> parents);
    std::vector<glm::vec3> SmoothPath(std::vector<vec3> cPoints);
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

    // getters
    const std::vector<glm::vec3>& GetPath() { return final_smooth_points_;}

  private:
    NavMesh* navMesh_;
    Pathfinder* pathfinder_;
    std::vector<glm::vec3> final_smooth_points_;
};

