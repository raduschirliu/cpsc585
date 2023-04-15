#include <rapidjson/fwd.h>

#include <glm/glm.hpp>
#include <vector>

struct CheckpointEntry
{
    glm::vec3 position;
    glm::vec3 orientation;
    glm::vec3 size;

    bool Deserialize(const rapidjson::Value& node);
};

class Checkpoints
{
  public:
    static void LoadCheckpointFile();
    static const std::vector<CheckpointEntry>& GetCheckpoints();
};