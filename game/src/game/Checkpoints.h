#include "GameApp.h"

class Checkpoints
{
  public:
    static void LoadCheckpointFile();
    static std::vector<std::pair<glm::vec3, glm::vec3>>& GetCheckpoints();
};