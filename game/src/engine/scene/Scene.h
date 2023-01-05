#pragma once

#include <string>
#include <vector>

#include "engine/scene/GameObject.h"

class Scene
{
  private:
    std::string name_;
    std::vector<GameObject> objects_;
};