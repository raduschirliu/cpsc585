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
    // auto node1 = *navMesh_->nodes_->find(1)->second;
    if (pathfinder_ && navMesh_)
        pathfinder_->Search(navMesh_->nodes_->find(13)->second,
                            navMesh_->nodes_->find(23)->second);

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
    file.open("resources/models/track/track3-4(navmesh).obj", std::ios::in);
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
            std::vector<float>
                temp_vertex;  // so that after all the points are read, we
                              // can add it to the main vertex
            std::stringstream ss(s);
            std::string word;
            while (ss >> word)
            {
                if (word == "v")
                {
                    // ignore
                }
                else
                {
                    temp_vertex.push_back(std::stof(word));
                }
            }
            // now add this to the overall vertices.
            all_vertices_.push_back(
                glm::vec3(temp_vertex[0], temp_vertex[1], temp_vertex[2]));
        }
    }

    Log::info("Total vertex points : {}", all_vertices_.size());
}

NavMesh::NavMesh()
{
    // Read the file which has all the vertices so that we can generate navmesh
    // easily.
    ReadVertices();

    // to store the nodes there are in the map.
    this->nodes_ = new std::map<unsigned int, Node*>();

    for (int i = 0; i < all_vertices_.size() - 2; i += 3)
    {
        auto& v1 = all_vertices_[i];
        auto& v2 = all_vertices_[i + 1];
        auto& v3 = all_vertices_[i + 2];
        Node* temp_node = new Node(node_index_, v1, v2, v3);
        all_nodes.push_back(temp_node);

        // add this node to the map we have for nodes
        this->nodes_->insert({temp_node->id_, temp_node});
        node_index_++;  // so that id can remain unique.
    }

    // making the connections for the nodes, i.e. the nodes know which node is
    // connected to what node.
    for (int i = 0; i < this->nodes_->size() - 1; i++)
    {
        Node* n1 = all_nodes[i];
        Node* n2 = all_nodes[i + 1];
        n1->connections_->emplace_back(std::make_pair(Cost(n1, n2), n2));
        //std::cout << i << " " << i+1 << std::endl;
    }

    {
        // // first row of vertices
        // vec3 v1(0.f, 0.f, 10.f);  // this is also the starting position of
        // the car. vec3 v2(66.5f, 0.f, 10.f); vec3 v3(133.0f, 0.f, 10.f); vec3
        // v4(199.5f, 0.f, 10.f); vec3 v5(266.f, 0.f, 10.f);

        // // second row of vertices.
        // vec3 v6(0.f, 0.f, -82.f);
        // vec3 v7(66.5f, 0.f, -82.f);
        // vec3 v8(133.0f, 0.f, -82.f);
        // vec3 v9(199.5f, 0.f, -82.f);
        // vec3 v10(266.f, 0.f, -82.f);

        // // third row of vertices.
        // vec3 v11(0.f, 0.f, -174.f);
        // vec3 v12(66.5f, 0.f, -174.f);
        // vec3 v13(133.0f, 0.f, -174.f);
        // vec3 v14(199.5f, 0.f, -174.f);
        // vec3 v15(266.f, 0.f, -174.f);

        // // fourth row of vertices.
        // vec3 v16(0.f, 0.f, -266.f);
        // vec3 v17(66.5f, 0.f, -266.f);
        // vec3 v18(133.0f, 0.f, -266.f);
        // vec3 v19(199.5f, 0.f, -266.f);
        // vec3 v20(266.f, 0.f, -266.f);

        // // using the above vertices to make our 24 triangles / nodes.
        // Node* n1 = new Node(0, v1, v6, v7);
        // Node* n2 = new Node(1, v1, v2, v7);

        // Node* n3 = new Node(2, v2, v7, v8);
        // Node* n4 = new Node(3, v2, v3, v7);

        // Node* n5 = new Node(4, v3, v8, v9);
        // Node* n6 = new Node(5, v3, v4, v9);

        // Node* n7 = new Node(6, v4, v9, v10);
        // Node* n8 = new Node(7, v4, v5, v10);

        // Node* n9 = new Node(8, v6, v11, v12);
        // Node* n10 = new Node(9, v6, v7, v12);

        // Node* n11 = new Node(10, v7, v12, v13);
        // Node* n12 = new Node(11, v7, v8, v13);

        // Node* n13 = new Node(12, v8, v13, v14);
        // Node* n14 = new Node(13, v8, v9, v14);

        // Node* n15 = new Node(14, v9, v14, v15);
        // Node* n16 = new Node(15, v9, v10, v15);

        // Node* n17 = new Node(16, v11, v16, v17);
        // Node* n18 = new Node(17, v11, v12, v17);

        // Node* n19 = new Node(18, v12, v17, v18);
        // Node* n20 = new Node(19, v12, v13, v18);

        // Node* n21 = new Node(20, v13, v18, v19);
        // Node* n22 = new Node(21, v13, v14, v19);

        // Node* n23 = new Node(22, v14, v19, v20);
        // Node* n24 = new Node(23, v14, v15, v20);

        // // create map of nodes.
        // this->nodes_->insert({n1->id_, n1});
        // this->nodes_->insert({n2->id_, n2});
        // this->nodes_->insert({n3->id_, n3});
        // this->nodes_->insert({n4->id_, n4});
        // this->nodes_->insert({n5->id_, n5});
        // this->nodes_->insert({n6->id_, n6});
        // this->nodes_->insert({n7->id_, n7});
        // this->nodes_->insert({n8->id_, n8});
        // this->nodes_->insert({n9->id_, n9});
        // this->nodes_->insert({n10->id_, n10});
        // this->nodes_->insert({n11->id_, n11});
        // this->nodes_->insert({n12->id_, n12});
        // this->nodes_->insert({n13->id_, n13});
        // this->nodes_->insert({n14->id_, n14});
        // this->nodes_->insert({n15->id_, n15});
        // this->nodes_->insert({n16->id_, n16});
        // this->nodes_->insert({n17->id_, n17});
        // this->nodes_->insert({n18->id_, n18});
        // this->nodes_->insert({n19->id_, n19});
        // this->nodes_->insert({n20->id_, n20});
        // this->nodes_->insert({n21->id_, n21});
        // this->nodes_->insert({n22->id_, n22});
        // this->nodes_->insert({n23->id_, n23});
        // this->nodes_->insert({n24->id_, n24});

        // n1->connections_->emplace_back(std::make_pair(Cost(n1, n2), n2));

        // n2->connections_->emplace_back(std::make_pair(Cost(n2, n3), n3));
        // // n2->connections_->emplace_back(std::make_pair(  Cost(n2, n1),
        // n1));

        // n3->connections_->emplace_back(std::make_pair(Cost(n3, n4), n4));
        // // n3->connections_->emplace_back(std::make_pair(  Cost(n3, n2),
        // n2));

        // n4->connections_->emplace_back(std::make_pair(Cost(n4, n5), n5));
        // // n4->connections_->emplace_back(std::make_pair(  Cost(n4, n3),
        // n3));

        // n5->connections_->emplace_back(std::make_pair(Cost(n5, n6), n6));
        // // n5->connections_->emplace_back(std::make_pair(  Cost(n5, n4),
        // n4));

        // n6->connections_->emplace_back(std::make_pair(Cost(n6, n7), n7));
        // // n6->connections_->emplace_back(std::make_pair(  Cost(n6, n5),
        // n5));

        // n7->connections_->emplace_back(std::make_pair(Cost(n7, n14), n14));
        // // n7->connections_->emplace_back(std::make_pair(  Cost(n7, n6),
        // n6));

        // n14->connections_->emplace_back(std::make_pair(Cost(n14, n13), n13));
        // // n14->connections_->emplace_back(std::make_pair(  Cost(n14, n7),
        // n7));

        // n13->connections_->emplace_back(std::make_pair(Cost(n13, n22), n22));
        // // n13->connections_->emplace_back(std::make_pair(  Cost(n13, n14),
        // n14));

        // n22->connections_->emplace_back(std::make_pair(Cost(n22, n21), n21));
        // // n22->connections_->emplace_back(std::make_pair(  Cost(n22, n13),
        // n13));

        // n21->connections_->emplace_back(std::make_pair(Cost(n21, n20), n20));
        // // n21->connections_->emplace_back(std::make_pair(  Cost(n21, n22),
        // n22));

        // n20->connections_->emplace_back(std::make_pair(Cost(n20, n11), n11));
        // // n20->connections_->emplace_back(std::make_pair(  Cost(n20, n21),
        // n21));

        // n11->connections_->emplace_back(std::make_pair(Cost(n11, n12), n12));
        // // n11->connections_->emplace_back(std::make_pair(  Cost(n11, n20),
        // n20));

        // n12->connections_->emplace_back(std::make_pair(Cost(n12, n13), n13));
        // // n12->connections_->emplace_back(std::make_pair(  Cost(n12, n11),
        // n11));

        // n13->connections_->emplace_back(std::make_pair(Cost(n13, n23), n23));
        // // n13->connections_->emplace_back(std::make_pair(  Cost(n13, n12),
        // n12));

        // n23->connections_->emplace_back(std::make_pair(Cost(n23, n24), n24));
        // // n23->connections_->emplace_back(std::make_pair(  Cost(n23, n13),
        // n13));
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
    std::vector<glm::vec3> smoothPath;
    smoothPath = SmoothPath(bPath);

    // storing everything in path
    for (auto& i : smoothPath) this->path_->push(i);
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