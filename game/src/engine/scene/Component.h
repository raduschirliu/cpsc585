#pragma once

#include <string_view>

#include "engine/core/debug/Assert.h"

class IComponent
{
  public:
    virtual void Init() = 0;
    virtual std::string_view GetName() const = 0;
};

STATIC_ASSERT_INTERFACE(IComponent);