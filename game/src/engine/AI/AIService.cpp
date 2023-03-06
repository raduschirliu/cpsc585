#include "AIService.h"

#include <fstream>
#include <set>
#include <sstream>
#include <string>

#include "engine/core/debug/Log.h"

//-------------------
// Custom overloads
//-------------------
std::ostream& operator<<(std::ostream& os, const NavMesh::Node& node)
{
    os << "node id: " << node.id_ << std::endl
       << node.v0_ << " " << node.v1_ << " " << node.v2_;
    os << std::endl << "Centroid: " << node.centroid_;
    return os;
}

AIService::AIService()
{
}

void AIService::OnInit()
{
    // create the basic navmesh, with all the quadrants and triangles
    // initialized
    navMesh_ = new NavMesh();
    pathfinder_ = new Pathfinder(navMesh_);

    Log::debug("AIService initialized");
}

void AIService::OnStart(ServiceProvider& service_provider)
{
    bool bvalue = false;
    // auto node1 = *navMesh_->nodes_->find(1)->second;
    if (pathfinder_ && navMesh_)
        bvalue = pathfinder_->Search(navMesh_->nodes_->find(0)->second,
                                     navMesh_->nodes_->find(331)->second);

    else
    {
        Log::error("Could not start the game, issue: AIService.cpp");
    }

    // points from one corner to other defined above.
    while (!this->pathfinder_->path_->empty())
    {
        final_smooth_points_.push_back(this->pathfinder_->path_->top());
        // std::cout << this->pathfinder_->path_->top() << std::endl;
        this->pathfinder_->path_->pop();
    }
}

void AIService::OnUpdate()
{
}

void AIService::OnCleanup()
{
}

std::string_view AIService::GetName() const
{
    return "AI Service";
}

//--------------------
// Node
//--------------------
NavMesh::Node::Node(unsigned int id, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2)
{
    this->id_ = id;
    this->v0_ = v0;
    this->v1_ = v1;
    this->v2_ = v2;
    this->centroid_ = Get_Centroid(this);
    this->connections_ = new std::vector<std::pair<float, Node*>>();
}

//-------------------
// NavMesh
// ------------------

void NavMesh::ReadVertices()
{
    std::fstream file;
    file.open("resources/models/track/track3-4navmesh.obj", std::ios::in);
    if (!file)
    {
        Log::error("Cannot open the navmesh file.");
    }
    else
    {
        // perform the file thing we want here.
        std::string s;
        while (std::getline(file, s))
        {
            if (s[0] == '#' || s[0] == 's' || s[0] == 'o' || s[0] == 'm' || s[0] == 'u')
                continue;
            std::vector<float>
                temp_vertex;  // so that after all the points are read, we
                              // can add it to the main vertex
            std::vector<int> temp_face;
            std::stringstream ss(s);
            std::string word;
            int index = 0;
            bool face_vertex_bool =
                false;  // false means its vertex information and true means its
                        // face information
            while (ss >> word)
            {
                if (word == "v")
                {
                    face_vertex_bool = false;
                }
                else if (word == "f")
                {
                    face_vertex_bool = true;
                }
                if (word != "v" && !face_vertex_bool)
                {
                    
                        temp_vertex.push_back(std::stof(word) + 0.f);
                    
                }
                else if (word != "f" && face_vertex_bool)
                {
                    temp_face.push_back(std::stoi(word));
                }
            }
            // now add this to the overall vertices.
            if (!face_vertex_bool)
                all_vertices_.push_back(
                    glm::vec3(temp_vertex[0], temp_vertex[1], temp_vertex[2]));
            else
                face_positions_.push_back(
                    glm::vec3(temp_face[0], temp_face[1], temp_face[2]));
        }
    }
}

NavMesh::NavMesh()
{
    // Read the file which has all the vertices so that we can generate navmesh
    // easily.
    ReadVertices();

    // to store the nodes there are in the map.
    this->nodes_ = new std::map<unsigned int, Node*>();

    // getting all the vertices first.
    for (int i = 0; i < face_positions_.size(); i++)
    {
        auto& v1 = all_vertices_[static_cast<int>(face_positions_[i].x) -1];
        auto& v2 = all_vertices_[static_cast<int>(face_positions_[i].y) -1 ];
        auto& v3 = all_vertices_[static_cast<int>(face_positions_[i].z) -1];
        Log::debug("{} : value {}", static_cast<int>(face_positions_[i].x), v1.x);
        Log::debug("{} : value {}", static_cast<int>(face_positions_[i].y), v2.x);
        Log::debug("{} : value {}", static_cast<int>(face_positions_[i].z), v3.x);
        Node* temp_node = new Node(node_index_, v1, v2, v3);
        all_nodes.push_back(temp_node);

        // add this node to the map we have for nodes
        this->nodes_->insert({temp_node->id_, temp_node});
        node_index_++;  // so that id can remain unique.
    }

    // making the connections for the nodes, i.e. the nodes know which node is
    // connected to what node.
    for (int i = 0; i < this->nodes_->size() - 2; i++)
    {
        Node* n1 = all_nodes[i];
        Node* n2 = all_nodes[i + 1];
        Node* n3 = all_nodes[i + 2];
        // Node* n6 = all_nodes[i + 5];
        // Node* n7 = all_nodes[i + 6];
        // Node* n8 = all_nodes[i + 7];
        n1->connections_->emplace_back(std::make_pair(Cost(n1, n2), n2));
        n1->connections_->emplace_back(std::make_pair(Cost(n1, n3), n3));
        // n1->connections_->emplace_back(std::make_pair(Cost(n1, n6), n6));
        // n1->connections_->emplace_back(std::make_pair(Cost(n1, n7), n7));
        // n1->connections_->emplace_back(std::make_pair(Cost(n1, n8), n8));
        // n1->size_ = (n1->v0_.x * (n1->v1_.z - n1->v2_.z)) +
        //             (n1->v1_.x * (n1->v2_.z - n1->v0_.z)) +
        //             (n1->v2_.x * (n1->v0_.z - n1->v1_.z)) / 2;
        // // std::cout << i << " " << i+1 << std::endl;
        // std::cout << std::endl
        //           << ((n1->v0_.x * (n1->v1_.z - n1->v2_.z)) +
        //               (n1->v1_.x * (n1->v2_.z - n1->v0_.z)) +
        //               (n1->v2_.x * (n1->v0_.z - n1->v1_.z))) /
        //                  2;
    }
}

float NavMesh::Cost(Node* src, Node* dest)
{
    // Get the centroid of the nodes
    glm::vec3 srcCenter = src->Get_Centroid(src);
    glm::vec3 destCenter = src->Get_Centroid(dest);

    // euclidean distance
    float dx = glm::abs(srcCenter.x - destCenter.x);
    float dy = glm::abs(srcCenter.y - destCenter.y);
    float dz = glm::abs(srcCenter.z - destCenter.z);

    return glm::sqrt((dx * dx) + (dy * dy) + (dz * dz));
}

bool Pathfinder::Search(NavMesh::Node* src, NavMesh::Node* dest)
{
    if (!src)
    {
        Log::error("Src is a null pointer in AIService");
        return false;
    }
    if (!dest)
    {
        Log::error("Dest is a null pointer in AIService");
        return false;
    }
    if (IsDestination(src, dest))
    {
        Log::info("Already at the location");
        return true;
    }

    // declare data structures
    std::map<unsigned int, bool> explored;  // visited nodes
    std::set<std::pair<float, NavMesh::Node*>> frontier;
    std::map<unsigned int, unsigned int> parents;

    // start by adding the src node to the fronteier with 0   Cost
    frontier.insert(std::make_pair(0.f, src));

    // also add it to the explored list as false
    explored.insert({frontier.begin()->second->id_, false});

    // add src node to parents list
    parents.insert({src->id_, src->id_});

    // main loop
    while (!frontier.empty())
    {
        // save a reference to the first entry in the frontier and remove it
        std::pair<float, NavMesh::Node*> p = *frontier.begin();
        frontier.erase(frontier.begin());

        // Add that to the explored list
        explored.find(p.second->id_)->second = true;

        float gNew = 0;
        float hNew = 0;
        float fNew = 0;

        // Initialize connections in explored list
        for (unsigned int i = 0; i < p.second->connections_->size(); i++)
        {
            if (explored.find(p.second->connections_->at(i).second->id_) ==
                explored.end())
            {
                explored.insert(
                    {p.second->connections_->at(i).second->id_, false});
            }
        }

        for (unsigned i = 0; i < p.second->connections_->size(); i++)
        {
            std::pair<float, NavMesh::Node*> test =
                p.second->connections_->at(i);

            // check if this is the destination
            if (IsDestination(test.second, dest))
            {
                parents.insert_or_assign(test.second->id_, p.second->id_);
                this->TracePath(test.second, dest, parents);
                return true;
            }
            else if (!explored.find(test.second->id_)->second)
            {
                // update   Cost,
                gNew = p.first + test.first;               // Actual   Cost
                hNew = CalculateHCost(test.second, dest);  // Heuristic   Cost
                fNew = gNew + hNew;
            }

            // Add to the frontier
            frontier.insert(std::make_pair(fNew, test.second));
            parents.insert_or_assign(test.second->id_, p.second->id_);
        }
    }
    Log::error("Destination cell cannot be found");
    return false;
}

Pathfinder::Pathfinder(NavMesh* navMesh)
{
    this->navMesh_ = navMesh;
    this->path_ = new std::stack<vec3>;
}

bool Pathfinder::IsDestination(NavMesh::Node* src, NavMesh::Node* dest)
{
    if (src->id_ == dest->id_)
        return true;
    else
        return false;
}

float Pathfinder::CalculateHCost(NavMesh::Node* src, NavMesh::Node* dest)
{
    // Get the centroid of the nodes
    glm::vec3 srcCenter = src->Get_Centroid(src);
    glm::vec3 destCenter = src->Get_Centroid(dest);

    // euclidean distance
    float dx = glm::abs(srcCenter.x - destCenter.x);
    float dy = glm::abs(srcCenter.y - destCenter.y);
    float dz = glm::abs(srcCenter.z - destCenter.z);

    return glm::sqrt((dx * dx) + (dy * dy) + (dz * dz));
}

void Pathfinder::TracePath(NavMesh::Node* src, NavMesh::Node* dest,
                           std::map<unsigned int, unsigned int> parents)
{
    std::vector<glm::vec3> bPath;

    while (!this->path_->empty())
    {
        this->path_->pop();
    }
    unsigned int temp = dest->id_;
    while (temp != parents.find(temp)->second)
    {
        bPath.push_back(this->navMesh_->nodes_->find(temp)->second->centroid_);
        temp = parents.find(temp)->second;
    }
    bPath.push_back(this->navMesh_->nodes_->find(temp)->second->centroid_);

    // smotth down this path.
    std::vector<glm::vec3> smoothPath = bPath;
    //smoothPath = SmoothPath(bPath);

    std::ofstream points_output("NavMeshPointsNEWNEW.obj");
    // storing everything in path
    // for (int i = smoothPath.size() - 1; i > 0; i--)
    for (int i = 0; i < smoothPath.size() - 1; i++)
    {
        this->path_->push(smoothPath[i]);
        points_output << "v " << smoothPath[i].x << " " << smoothPath[i].y << " " << smoothPath[i].z << std::endl;
    }
    points_output.close();
}

std::vector<glm::vec3> Pathfinder::SmoothPath(std::vector<glm::vec3> cPoints)
{
    // do not care about smoothing the path if size is less than 3 points
    if (cPoints.size() < 3)
        return {};
    std::vector<glm::vec3> iVerts;
    int degree = cPoints.size();

    // clear previous path.
    while (!this->path_->empty())
    {
        this->path_->pop();
    }

    float c2 = 1.f / 2.f;
    float c0 = 1.f / 4.f;
    float c1 = 3.f / 4.f;

    for (unsigned int u = 0; u < 4; u++)
    {
        iVerts.push_back(cPoints[0]);
        iVerts.push_back(c2 * cPoints[0] + c2 * cPoints[1]);

        for (int i = 0; i < degree - 2; i++)
        {
            iVerts.push_back(c0 * cPoints[i] + c1 * cPoints[i + 1]);
            iVerts.push_back(c1 * cPoints[i] + c0 * cPoints[i + 1]);
        }

        iVerts.push_back(c2 * cPoints[degree - 1] + c2 * cPoints[degree - 2]);
        iVerts.push_back(cPoints[degree - 1]);

        cPoints = iVerts;
        iVerts.clear();
        degree = cPoints.size();
    }
    return cPoints;
}

glm::vec3 NavMesh::Node::Get_Centroid(NavMesh::Node* node)
{
    return (node->v0_ + node->v1_ + node->v2_) / 3.f;
}