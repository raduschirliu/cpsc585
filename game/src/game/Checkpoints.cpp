#include "Checkpoints.h"

#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <sstream>
#include <unordered_set>
#include <utility>

std::vector<std::pair<glm::vec3, glm::vec3>> Checkpoints::GetCheckpoints()
{
    // import the checkpoints from the navmesh points.
    std::fstream file;
    file.open("resources/checkpoints/checkpoints.obj", std::ios::in);
    if (!file)
    {
        return {};
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
            std::stringstream ss(s);
            std::string word;
            while (ss >> word)
            {
                if (word == "v")
                {
                    continue;
                }
                else
                {
                    temp_vertex.push_back(std::stof(word));
                }
            }
            kCheckpoints.push_back({
                glm::vec3(temp_vertex[0], temp_vertex[1], temp_vertex[2]),
                glm::vec3(0, 0, 0)});
        }
    }

    // sort the checkpoints based on the distance between them, as they are
    // random right now. using the same ai approach.
    int starting_index = 82;
    kSortedCheckpoints.push_back(kCheckpoints[starting_index]);
    std::unordered_set<int> traced_index;
    traced_index.insert(starting_index);

    while (kCheckpoints.size() != traced_index.size())
    {
        int min_index = 0;
        float min_distance = std::numeric_limits<float>::max();
        // find the smallest path which not has been traversed yet.
        for (int i = 0; i < kCheckpoints.size(); i++)
        {
            if (traced_index.find(i) == traced_index.end())
            {
                float dist = glm::distance(kCheckpoints[i].first,
                                           kCheckpoints[starting_index].first);
                if (min_distance > dist)
                {
                    min_index = i;
                    min_distance = dist;
                }
            }
        }
        // now as we found the closest to it, removing the previous index from
        // kCheckpoint and iterating again
        traced_index.insert(min_index);
        starting_index = min_index;
        kSortedCheckpoints.push_back(kCheckpoints[starting_index]);
    }

    // fixing the orientation manually
    for (int i = 5; i <= 9; i++)
    {
        kSortedCheckpoints[i].second = glm::vec3(0, -90, 0);
    }

    kSortedCheckpoints.erase(kSortedCheckpoints.begin() + 12);

    for (int i = 12; i <= 14; i++)
    {
        kSortedCheckpoints[i].second = glm::vec3(0, 90, 0);
    }

    kSortedCheckpoints.erase(kSortedCheckpoints.begin() + 15);
    kSortedCheckpoints.erase(kSortedCheckpoints.begin() + 16);
    kSortedCheckpoints.erase(kSortedCheckpoints.begin() + 20);
    kSortedCheckpoints.erase(kSortedCheckpoints.begin() + 51);
    kSortedCheckpoints.erase(kSortedCheckpoints.begin() + 52);
    kSortedCheckpoints.erase(kSortedCheckpoints.begin() + 53);
    kSortedCheckpoints.erase(kSortedCheckpoints.begin() + 24);
    kSortedCheckpoints.erase(kSortedCheckpoints.begin() + 25);
    kSortedCheckpoints.erase(kSortedCheckpoints.begin() + 27);
    kSortedCheckpoints.erase(kSortedCheckpoints.begin() + 29);
    kSortedCheckpoints.erase(kSortedCheckpoints.begin() + 31);
    kSortedCheckpoints.erase(kSortedCheckpoints.begin() + 26);
    kSortedCheckpoints.erase(kSortedCheckpoints.begin() + 30);
    kSortedCheckpoints.erase(kSortedCheckpoints.begin() + 27);
    kSortedCheckpoints.erase(kSortedCheckpoints.begin() + 33);
    kSortedCheckpoints.erase(kSortedCheckpoints.begin() + 34);
    kSortedCheckpoints.erase(kSortedCheckpoints.begin() + 40);

    for (int i = 20; i <= 30; i++)
    {
        kSortedCheckpoints[i].second = glm::vec3(0, -90, 0);
    }
    kSortedCheckpoints.erase(kSortedCheckpoints.begin() + 35);
    for (int i = 40; i <= 42; i++)
    {
        kSortedCheckpoints[i].second = glm::vec3(0, -90, 0);
    }

    kSortedCheckpoints.erase(kSortedCheckpoints.begin() + 46);
    kSortedCheckpoints.erase(kSortedCheckpoints.begin() + 47);
    kSortedCheckpoints.erase(kSortedCheckpoints.begin() + 48);

    for (int i = 46; i <= 49; i++)
    {
        kSortedCheckpoints[i].second = glm::vec3(0, -90, 0);
    }

    kSortedCheckpoints.erase(kSortedCheckpoints.begin() + 51);
    kSortedCheckpoints.erase(kSortedCheckpoints.begin() + 52);

    for (int i = 50; i <= 56; i++)
    {
        kSortedCheckpoints[i].second = glm::vec3(0, -90, 0);
    }
    kSortedCheckpoints.erase(kSortedCheckpoints.begin() + 56);
    kSortedCheckpoints.erase(kSortedCheckpoints.begin() + 55);
    kSortedCheckpoints.erase(kSortedCheckpoints.begin() + 58);
    kSortedCheckpoints.erase(kSortedCheckpoints.begin() + 58);

    // for (int i = 35; i <= 37; i++)
    // {
    //     kSortedCheckpoints[i].second = (glm::vec3(130, 0, 0));
    // }
    return kSortedCheckpoints;
}