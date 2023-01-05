#pragma once

#include <memory>
#include <string>
#include <vector>

#include "engine/scene/Component.h"

class GameObject
{
  private:
    size_t id_;
    std::string name_;
    std::vector<std::unique_ptr<Component>> components_;
};