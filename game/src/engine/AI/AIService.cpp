#include "AIService.h"

#include <fstream>
#include <sstream>
#include <string>

#include "engine/core/debug/Log.h"

AIService::AIService()
{
}

std::vector<glm::vec3> AIService::GetPath()
{
    std::ofstream points_output("NavMeshPointsNEWNEW.obj");
    std::vector<glm::vec3> path;
    for (int i = 0; i < faces_.size(); i++)
    {
        path.push_back(faces_[i].centroid);
        points_output << "v " << faces_[i].centroid.x << " "
                      << faces_[i].centroid.y << " " << faces_[i].centroid.z
                      << std::endl;
    }
    points_output.close();
    return path;
}

void AIService::OnInit()
{
    Log::debug("AIService initialized");
    ReadVertices();
}

void AIService::OnStart(ServiceProvider& service_provider)
{
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

void AIService::ReadVertices()
{
    std::fstream file;
    file.open("resources/models/track/track3-7navmesh2.obj", std::ios::in);
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
            if (s[0] == '#' || s[0] == 's' || s[0] == 'o' || s[0] == 'm' ||
                s[0] == 'u' || s[0] == 'l')
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
                vertices_.push_back(
                    glm::vec3(temp_vertex[0], temp_vertex[1], temp_vertex[2]));
            else
                faces_index_.push_back(
                    glm::vec3(temp_face[0], temp_face[1], temp_face[2]));
        }
    }

    // now we have all the information of faces and vertices.
    for (int i = 0; i < faces_index_.size(); i++)
    {
        auto& v1 = vertices_[static_cast<int>(faces_index_[i].x) - 1];
        auto& v2 = vertices_[static_cast<int>(faces_index_[i].y) - 1];
        auto& v3 = vertices_[static_cast<int>(faces_index_[i].z) - 1];
        faces_.push_back(Faces(v1, v2, v3));
    }

    // putting the first face in. used for comparison.
    sorted_faces_.push_back(faces_[0]);

    // sort the faces so that they are one after the other.
    for (int i = 0; i < faces_.size(); i++)
    {
        // find the closest to this face
        int lowest_index_ = 0;
        float lowest_distance_ = INT_MAX;

        for (int j = i + 1; j < faces_.size(); j++)
        {
            int dist = glm::distance(faces_[i].centroid, faces_[j].centroid);
            if (lowest_distance_ > dist)
            {
                // closest neighbour of this till now.
                lowest_distance_ = dist;
                lowest_index_ = j;
            }
        }
        // now we have the closest point, adding that.
        sorted_faces_.push_back(faces_[lowest_index_]);
    }
}
