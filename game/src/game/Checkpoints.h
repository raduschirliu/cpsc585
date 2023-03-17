#include "GameApp.h"

class Checkpoints : public GameApp
{
  private:
    std::vector<std::pair<glm::vec3, glm::vec3>> kCheckpoints;
    std::vector<std::pair<glm::vec3, glm::vec3>> kSortedCheckpoints;

  public:
    std::vector<std::pair<glm::vec3, glm::vec3>> GetCheckpoints();
};